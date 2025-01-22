#!/bin/bash

APP_HOME=/root/service/libhv_transform_http
APP_NAME=libhv_http

#PID  代表是PID文件
PID=$APP_NAME\.pid

#使用说明
usage() {
  echo "Usage: sh 执行脚本.sh [start|stop|restart|status]"
  exit 1
}

#检查程序是否在运行
is_exist() {
  pid=$(ps -ef | grep -w $APP_NAME | grep -v grep | awk '{print $2}')
  #如果不存在返回0，存在返回1
  if [ -z "${pid}" ]; then
    return 0
  else
    return 1
  fi
}

#启动方法
start() {
  is_exist
  if [ $? -eq "1" ]; then
    echo ">>> ${APP_NAME}(${pid}) is already running <<<"
  else
    cd $APP_HOME
    nohup ./$APP_NAME >/dev/null 2>&1 &
    echo $! >$PID
    echo ">>> start ${APP_NAME}($!) successfully <<<"
  fi
}

#停止方法
stop() {
  is_exist
  if [ $? -eq "0" ]; then
    echo ">>> ${APP_NAME} is not running <<<"
    return
  fi

  cd $APP_HOME
  pidf=$(cat $PID)
  echo ">>> begin kill ${APP_NAME}(${pidf}) <<<"
  kill -9 $pidf

  is_exist
  if [ $? -eq "1" ]; then
    sleep 3
  fi

  echo ">>> ${APP_NAME}(${pidf}) is stopped <<<"

  rm -f $PID
}

#输出运行状态
status() {
  is_exist
  if [ $? -eq "1" ]; then
    echo ">>> ${APP_NAME}(${pid}) is running. <<<"
  else
    echo ">>> ${APP_NAME}(${pid}) is not running <<<"
  fi
}

#重启
restart() {
  stop
  start
}

#根据输入参数
case "$1" in
"start")
  start
  ;;
"stop")
  stop
  ;;
"status")
  status
  ;;
"restart")
  restart
  ;;
*)
  usage
  ;;
esac
exit 0
