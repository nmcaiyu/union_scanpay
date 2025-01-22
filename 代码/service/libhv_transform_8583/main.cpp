/*
 *
 * @build   make examples
 * @server  bin/multi-acceptor-processes 1234
 * @client  bin/nc 127.0.0.1 1234
 *          nc     127.0.0.1 1234
 *          telnet 127.0.0.1 1234
 */
#include "action.h"
#include "config.h"
#include "hloop.h"
#include "hmain.h"
#include "hproc.h"
#include "hsocket.h"
#include "hthread.h"
#include "hv.h"
#include "mysql.h"
#include "slog.h"

// static int listenfd = INVALID_SOCKET;
static hloop_t* accept_loop = NULL;
static hloop_t** worker_loops = NULL;
unpack_setting_t unpack_setting;

static hloop_t* get_next_loop()
{
    static int s_cur_index = 0;
    if (s_cur_index == globalCFG.thread_num)
    {
        s_cur_index = 0;
    }
    return worker_loops[s_cur_index++];
}

static void on_recv(hio_t* io, void* buf, int readbytes)
{
    char szSendClient[MAXPACKLEN];
    int iSendClientLen = 0;
    BUFCLR(szSendClient);

    LOG_INFOSB((char*)buf, readbytes, "接收请求报文[%d]", readbytes);

    action((char*)buf, readbytes, szSendClient, &iSendClientLen);
    if (iSendClientLen > 0)
    {
        LOG_INFOSB(szSendClient, iSendClientLen, "发送返回报文[%d]", iSendClientLen);
        hio_write(io, szSendClient, iSendClientLen);
    }

    hio_close(io);
}

// static void on_accept(hio_t* io) {
//     char localaddrstr[SOCKADDR_STRLEN] = {0};
//     char peeraddrstr[SOCKADDR_STRLEN] = {0};
//     // printf("pid=%ld connfd=%d [%s] <= [%s]\n",
//     //         (long)hv_getpid(),
//     //         (int)hio_fd(io),
//     //         SOCKADDR_STR(hio_localaddr(io), localaddrstr),
//     //         SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));
// unpack_setting_t unpack_setting;
// memset(&unpack_setting, 0, sizeof(unpack_setting_t));
// unpack_setting.mode = UNPACK_BY_LENGTH_FIELD;
// unpack_setting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
// unpack_setting.fixed_length = 0;
// unpack_setting.delimiter_bytes = 0;
// unpack_setting.delimiter_bytes = 0;
// unpack_setting.body_offset = 2;
// unpack_setting.length_field_offset = 0;
// unpack_setting.length_field_bytes  = 2;
// unpack_setting.length_field_coding = ENCODE_BY_BIG_ENDIAN;
// hio_set_unpack(io, &unpack_setting);
//     hio_setcb_read(io, on_recv);
//     hio_read(io);
// }

// static void loop_proc(void* userdata) {
//     hloop_t* loop = hloop_new(HLOOP_FLAG_AUTO_FREE);
//     haccept(loop, listenfd, on_accept);
//     hloop_run(loop);
// }

// static HTHREAD_RETTYPE loop_thread(void* userdata) {
//     hloop_t* loop = hloop_new(HLOOP_FLAG_AUTO_FREE);
//     haccept(loop, listenfd, on_accept);
//     hloop_run(loop);
//     return 0;
// }

static void new_conn_event(hevent_t* ev)
{
    hloop_t* loop = ev->loop;
    hio_t* io = (hio_t*)hevent_userdata(ev);
    hio_attach(loop, io);

    char localaddrstr[SOCKADDR_STRLEN] = {0};
    char peeraddrstr[SOCKADDR_STRLEN] = {0};
    printf("tid=%ld connfd=%d [%s] <= [%s]\n", (long)hv_gettid(), (int)hio_fd(io), SOCKADDR_STR(hio_localaddr(io), localaddrstr),
           SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));
    hio_set_unpack(io, &unpack_setting);
    hio_setcb_read(io, on_recv);
    hio_read(io);
}

static void on_accept(hio_t* io)
{
    hio_detach(io);

    hloop_t* worker_loop = get_next_loop();
    hevent_t ev;
    memset(&ev, 0, sizeof(ev));
    ev.loop = worker_loop;
    ev.cb = new_conn_event;
    ev.userdata = io;
    hloop_post_event(worker_loop, &ev);
}

static HTHREAD_RETTYPE worker_thread(void* userdata)
{
    hloop_t* loop = (hloop_t*)userdata;
    hloop_run(loop);
    return 0;
}

static HTHREAD_RETTYPE accept_thread(void* userdata)
{
    hloop_t* loop = (hloop_t*)userdata;
    hio_t* listenio = hloop_create_tcp_server(loop, globalCFG.host, globalCFG.port, on_accept);
    if (listenio == NULL)
    {
        exit(1);
    }
    hloop_run(loop);
    return 0;
}

int main()
{
    if (readIni() != 0)
    {
        exit(1);
    }
    // proc_ctx_t ctx;
    // memset(&ctx, 0, sizeof(ctx));
    // ctx.proc = loop_proc;
    // for (int i = 0; i < globalCFG.process_num; ++i) {
    //     hproc_spawn(&ctx);
    // }

    // signal_init();
    // create_pidfile();
    // master_workers_run(worker_fn, (void*)listenfd, 4, 4, true);
    if (initMysql() != 0)
    {
        printf("初始化数据库失败\n");
        exit(1);
    }

    memset(&unpack_setting, 0, sizeof(unpack_setting_t));
    unpack_setting.mode = UNPACK_BY_LENGTH_FIELD;
    unpack_setting.package_max_length = DEFAULT_PACKAGE_MAX_LENGTH;
    unpack_setting.fixed_length = 0;
    unpack_setting.delimiter_bytes = 0;
    unpack_setting.delimiter_bytes = 0;
    unpack_setting.body_offset = 2;
    unpack_setting.length_field_offset = 0;
    unpack_setting.length_field_bytes = 2;
    unpack_setting.length_field_coding = ENCODE_BY_BIG_ENDIAN;

    worker_loops = (hloop_t**)malloc(sizeof(hloop_t*) * globalCFG.thread_num);
    for (int i = 0; i < globalCFG.thread_num; ++i)
    {
        worker_loops[i] = hloop_new(HLOOP_FLAG_AUTO_FREE);
        hthread_create(worker_thread, worker_loops[i]);
    }
    accept_loop = hloop_new(HLOOP_FLAG_AUTO_FREE);
    accept_thread(accept_loop);

    return 0;
}
