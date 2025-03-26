#include "action.h"

static size_t http_response(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    char** response_ptr = (char**)userp;

    if (*response_ptr == NULL) {
        *response_ptr = (char*)calloc(1, realsize + 1);
        if (*response_ptr == NULL) {
            printf("Not enough memory (calloc failed)\n");
            return 0;
        }
    } else {
        size_t old_size = strlen(*response_ptr);
        char* temp = (char*)realloc(*response_ptr, old_size + realsize + 1);
        if (temp == NULL) {
            printf("Not enough memory (realloc returned NULL)\n");
        free(*response_ptr);
            *response_ptr = NULL;
        return 0;
        }
        *response_ptr = temp;
    }

    memcpy(*response_ptr + strlen(*response_ptr), contents, realsize);
    (*response_ptr)[strlen(*response_ptr) + realsize] = '\0';
    return realsize;
}

std::string performHttpRequest(const std::string& url, const std::string& params) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("CURL初始化失败");
        return "";
    }

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, ("Content-Length: " + std::to_string(params.length())).c_str());

    char* response = NULL;
    char errbuf[CURL_ERROR_SIZE] = {0};

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, params.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_response);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

    CURLcode res = curl_easy_perform(curl);
    std::string result;
    if (res != CURLE_OK) {
        LOG_ERROR("HTTP请求失败: %s", errbuf);
    } else {
        if (response) {
            result = response;
            free(response);
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return result;
}

std::string sha256(const std::string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string bytes_to_hex(const unsigned char* bytes, size_t len) {
    std::stringstream ss;
    for (size_t i = 0; i < len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];
    }
    return ss.str();
}

std::vector<unsigned char> hex_to_bytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = (unsigned char) strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

int aes_ecb_decrypt(const unsigned char *ciphertext, int cipher_len, unsigned char *plaintext, unsigned char *key) {
    
    if (cipher_len % AES_BLOCK_SIZE != 0) {
        LOG_ERROR("解密长度不是16的整数倍，无法解密");
        return -1;
    }

    AES_KEY decryptKey;
    if (AES_set_decrypt_key(key, 128, &decryptKey) != 0) {
        LOG_ERROR("AES密钥设置失败");
        return -1;
    }

    int len = 0;
    while (len + AES_BLOCK_SIZE <= cipher_len) {
        AES_ecb_encrypt(ciphertext + len, plaintext + len, &decryptKey, AES_DECRYPT);
        len += AES_BLOCK_SIZE;
    }

    if (len > 0) {
        unsigned char padding_len = plaintext[len - 1];
        if (padding_len > 0 && padding_len <= AES_BLOCK_SIZE) {
            bool valid_padding = true;
            for (int i = 0; i < padding_len; i++) {
                if (plaintext[len - 1 - i] != padding_len) {
                    valid_padding = false;
                    break;
                }
            }
            if (valid_padding) {
                len -= padding_len;
            }
        }
    }

    return len;
}

std::string generateSign(const std::string &data) {
    std::string rawData = data + globalCFG.gtIV;
    LOG_INFO("待签名数据: %s", rawData.c_str());
    return sha256(rawData);
}

std::string buildCommonParam(const POSTRANS_GT_COMMON& common) {
    
    return "CUST_ID=" + common.CUST_ID + 
           "|#|TERM_SN=" + common.TERM_SN +
           "|#|POS_TOKEN=" + GetPosToken(common.TERM_SN) +
           "|#|POS_IMEI=" + common.POS_IMEI +
           "|#|SYSCOD=" + common.SYSCOD + 
           "|#|U_GPS_ADDREES=" + common.U_GPS_ADDREES +
           "|#|MOBMODEL=" + common.MOBMODEL + 
           "|#|TXNDAT=" + common.TXNDAT +
           "|#|TXNTIM=" + common.TXNTIM;
}

bool parseResponse(const std::string& decryptedText, std::string& respCode) {
    try {
        POSTRANS_GT_COMMON_RESPONSE data;
        xpack::json::decode(decryptedText, data);
        respCode = data.RSPCOD;
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("解析响应数据失败: %s", e.what());
        return false;
    }
}

bool doSignIn(POSTRANS_GT_COMMON& common) {
    LOG_INFO("开始执行自动签到流程获取新token");
    
    try {
        
        std::string commonParam = buildCommonParam(common);
        std::string signParam = commonParam + 
                               "|#|APP_VERSION=" + common.APP_VERSION + 
                               "|#|APP_TERMVERSION=" + common.APP_TERMVERSION +
                               "|#|ICC_ID=" + common.ICC_ID;
        
        std::string sText = "\"" + signParam + "\"";
        std::string sign = generateSign(signParam);
        
        std::string params = "sText=" + sText +
                           "&THIRD_ACCESS_CODE=" + common.THIRD_ACCESS_CODE +
                           "&sign=" + sign;
        
        std::string fullUrl = std::string(globalCFG.gtUrl) + "800502.po";
        LOG_INFO("签到URL: %s", fullUrl.c_str());
        
        // 发送签到请求
        std::string responseStr = performHttpRequest(fullUrl, params);
        if (responseStr.empty()) {
            LOG_ERROR("签到请求失败，未收到响应");
            return false;
        }
        
        std::vector<unsigned char> cipher_bytes = hex_to_bytes(responseStr);
        if (cipher_bytes.empty()) {
            LOG_ERROR("签到响应数据转换失败，响应原文: %s", responseStr.c_str());
            return false;
        }
        
        unsigned char plaintext[4096] = {0};
        int decrypted_len = aes_ecb_decrypt(cipher_bytes.data(), cipher_bytes.size(), plaintext, (unsigned char*)globalCFG.gtKey);
        if (decrypted_len <= 0) {
            LOG_ERROR("签到响应解密失败，响应长度: %zu", cipher_bytes.size());
            return false;
        }
        
        plaintext[decrypted_len] = '\0';
        std::string utf8_text((char*)plaintext, decrypted_len);
        LOG_INFO("签到解密后数据: %s", utf8_text.c_str());
        
        // 解析响应获取token
        try {
            POSTRANS_SIGN_RESPONSE signResponse;
            xpack::json::decode(utf8_text, signResponse);
            
            if (signResponse.RSPCOD != "000000") {
                LOG_ERROR("签到失败，响应码: %s, 响应信息: %s", 
                         signResponse.RSPCOD.c_str(), 
                         signResponse.RSPMSG.c_str());
                return false;
            }
            
            if (signResponse.POS_TOKEN.empty()) {
                LOG_ERROR("签到成功但未获取到token");
                return false;
            }
            
            // 保存新token
            common.POS_TOKEN = signResponse.POS_TOKEN;
            LOG_INFO("签到成功，获取新token: %s", signResponse.POS_TOKEN.c_str());
            
            // 更新token到数据库
            POS_TOKEN posToken;
            posToken.TERM_SN = common.TERM_SN;
            posToken.POS_TOKEN = signResponse.POS_TOKEN;
            if (UpdatePosToken(posToken) != 0) {
                LOG_WARN("数据库更新token失败，但仍将继续使用新token");
            }
            
            return true;
        } catch (const std::exception& e) {
            LOG_ERROR("解析签到响应失败: %s, 响应数据: %s", e.what(), utf8_text.c_str());
            return false;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("签到流程异常: %s", e.what());
        return false;
    } catch (...) {
        LOG_ERROR("签到流程中发生未知异常");
        return false;
    }
}

bool processTransaction(const std::string& fullParam, const std::string& endpoint, POSTRANS_GT_COMMON& common, 
                       char* sendbuf, int* sendLen, bool isRetry = false) {
    try {
        std::string sText = "\"" + fullParam + "\"";
        std::string sign = generateSign(fullParam);
        
        std::string params = "sText=" + sText +
                            "&THIRD_ACCESS_CODE=" + common.THIRD_ACCESS_CODE +
                            "&sign=" + sign;
        
        std::string fullUrl = std::string(globalCFG.gtUrl) + endpoint;
        LOG_INFO("请求URL: %s, 交易类型: %s", fullUrl.c_str(), endpoint.c_str());

        
        // 发送请求
        std::string responseStr = performHttpRequest(fullUrl, params);
        if (responseStr.empty()) {
            LOG_ERROR("请求失败，未收到响应");
            return false;
        }
        
        // 解密响应
        std::vector<unsigned char> cipher_bytes = hex_to_bytes(responseStr);
        if (cipher_bytes.empty()) {
            LOG_ERROR("响应数据转换失败，响应原文: %s", responseStr.c_str());
            return false;
        }
        
        unsigned char plaintext[4096] = {0};
        int decrypted_len = aes_ecb_decrypt(cipher_bytes.data(), cipher_bytes.size(), plaintext, (unsigned char*)globalCFG.gtKey);
        if (decrypted_len <= 0) {
            LOG_ERROR("解密失败，响应长度: %zu", cipher_bytes.size());
            return false;
        }
        
        plaintext[decrypted_len] = '\0';
        std::string utf8_text((char*)plaintext, decrypted_len);
        LOG_INFO("解密后数据: %s", utf8_text.c_str());
        
        // 检查响应码
        std::string respCode;
        if (parseResponse(utf8_text, respCode)) {
            LOG_INFO("响应码: %s", respCode.c_str());
            
            // 检查token是否过期
            if ((respCode == "008077" || respCode == "008777") && !isRetry) {
                LOG_INFO("Token已过期(RSPCOD=%s)，准备重新签到", respCode.c_str());
                
                // 执行签到获取新token
                if (doSignIn(common)) {
                    // 重新构建参数，替换token
                    std::string commonParam = buildCommonParam(common);
                    
                    // 更安全地提取特定参数部分
                    std::string specificParam;
                    size_t txntimPos = fullParam.find("|#|TXNTIM=");
                    if (txntimPos != std::string::npos) {
                        // 找到TXNTIM后的第一个|#|
                        size_t nextParamPos = fullParam.find("|#|", txntimPos + 10);
                        if (nextParamPos != std::string::npos) {
                            // 截取后面所有的特定参数
                            specificParam = fullParam.substr(nextParamPos + 3);
                        } else {
                            // 如果没找到后面的|#|，可能是参数格式有问题
                            LOG_ERROR("参数格式异常，无法提取特定参数");
                            return false;
                        }
                    } else {
                        // 如果没找到TXNTIM，可能是参数格式有问题
                        LOG_ERROR("参数格式异常，未找到TXNTIM字段");
                        return false;
                    }
                    
                    std::string newFullParam = commonParam + "|#|" + specificParam;
                    LOG_INFO("使用新token重试交易，新参数: %s", newFullParam.c_str());
                    
                    // 使用新token重试
                    return processTransaction(newFullParam, endpoint, common, sendbuf, sendLen, true);
                } else {
                    LOG_ERROR("重新签到失败，无法获取新token");
                    return false;
                }
            } else if (respCode != "000000" && !isRetry) {
                // 处理其他错误码，这里只记录日志，不做特殊处理
                LOG_WARN("交易返回非成功响应码: %s", respCode.c_str());
            }
        } else {
            LOG_ERROR("无法解析响应数据中的响应码");
        }
        
        // 处理响应数据
        size_t utf8_len = utf8_text.length();
        
        // 检查缓冲区大小是否足够
        if (utf8_len > 0xFFFF) {  // 超过2字节能表示的最大值
            LOG_ERROR("响应数据过大，无法放入缓冲区: %zu bytes", utf8_len);
            return false;
        }
        
        sendbuf[0] = utf8_len / 256;
        sendbuf[1] = utf8_len % 256;
        memcpy(sendbuf + 2, utf8_text.c_str(), utf8_len);
        *sendLen = utf8_len + 2;
        
        LOG_INFO("交易处理完成，返回数据长度: %zu bytes", utf8_len);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("处理交易异常: %s", e.what());
        return false;
    } catch (...) {
        LOG_ERROR("处理交易时发生未知异常");
        return false;
    }
}

void copyToSendBuffer(const std::string& errorMsg, char* sendbuf, int* sendLen) {
    size_t errorLen = errorMsg.length();
    sendbuf[0] = errorLen / 256;
    sendbuf[1] = errorLen % 256;
    memcpy(sendbuf + 2, errorMsg.c_str(), errorLen);
    *sendLen = errorLen + 2;
}

std::string md5Hash(const std::string& input) {
    unsigned char md[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)input.c_str(), input.size(), md);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)md[i];
    }

    return ss.str().substr(0, 24); // 取前24位
}

std::string pkcs5Padding(const std::string& data, size_t blockSize) {
    size_t padding = blockSize - (data.size() % blockSize);
    std::string padded = data;
    padded.append(padding, (char)padding);
    return padded;
}

std::string des3Encrypt(const std::string& key, const std::string& plaintext) {
    DES_key_schedule ks1, ks2, ks3;
    DES_cblock key1, key2, key3;

    memcpy(key1, key.c_str(), 8);
    memcpy(key2, key.c_str() + 8, 8);
    memcpy(key3, key.c_str() + 16, 8);

    DES_set_key_unchecked(&key1, &ks1);
    DES_set_key_unchecked(&key2, &ks2);
    DES_set_key_unchecked(&key3, &ks3);

    std::vector<unsigned char> out(plaintext.size());

    for (size_t i = 0; i < plaintext.size(); i += 8) {
        DES_ecb3_encrypt(
            (const_DES_cblock*)(plaintext.data() + i),
            (DES_cblock*)(out.data() + i),
            &ks1, &ks2, &ks3,
            DES_ENCRYPT);
    }

    return std::string(out.begin(), out.end());
}

std::string base64Encode(const std::string& input) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // 不加换行
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return result;
}

std::string encryptCardNumber(const std::string& cardNumber, const std::string& token) {
    // 1. 填充卡号
    std::string paddedCard = cardNumber;
    if (cardNumber.length() >= 16) {
        paddedCard = pkcs5Padding(cardNumber, 24);
    } else {
        paddedCard = pkcs5Padding(cardNumber, 8);
    }

    std::cout << "Padded Card: " << paddedCard << std::endl;

    // 2. 生成 MD5 并取前24位
    std::string md5Key = md5Hash(token);
    std::cout << "MD5 Key (24位): " << md5Key << std::endl;

    // 3. 3DES 加密
    std::string encrypted = des3Encrypt(md5Key, paddedCard);
    std::cout << "Encrypted Data: " << encrypted << std::endl;

    // 4. Base64 编码
    std::string encoded = base64Encode(encrypted);
    std::cout << "Base64 Encoded: " << encoded << std::endl;

    return encoded;
}

// 将PINBLK进行PKCS7填充
std::string processPINBLK(const std::string& pinblk) {
    if (pinblk.empty()) {
        return pinblk;
    }
    
    try {
        // 应用PKCS7填充
        size_t blockSize = AES_BLOCK_SIZE;
        size_t padding = blockSize - (pinblk.length() % blockSize);
        std::string paddedData = pinblk;
        for (size_t i = 0; i < padding; i++) {
            paddedData.push_back((char)padding);
        }
        
        return paddedData;
    } catch (const std::exception& e) {
        return pinblk;
    }
}

void processGT(char* recvbuf, int recvLen, char* sendbuf, int* sendLen, const POSTRANS_RESPONSE& response) {
    LOG_INFO("接收到GT请求: %s", response.transId.c_str());

    try {
        // 检查接收数据是否有效
        if (recvbuf == nullptr || recvLen <= 0) {
            LOG_ERROR("无效的请求数据");
            std::string errorMsg = "{\"RSPCOD\":\"XX\",\"RSPMSG\":\"无效的请求数据\"}";
            copyToSendBuffer(errorMsg, sendbuf, sendLen);
            return;
        }
        
        // 解析通用数据
        POSTRANS_GT_COMMON common;
        try {
            xpack::json::decode(recvbuf, common);
            LOG_INFO("解析common成功: TERM_SN=%s", common.TERM_SN.c_str());
        } catch (const std::exception& e) {
            LOG_ERROR("解析common失败: %s", e.what());
            std::string errorMsg = "{\"RSPCOD\":\"XX\",\"RSPMSG\":\"解析common失败: " + std::string(e.what()) + "\"}";
            copyToSendBuffer(errorMsg, sendbuf, sendLen);
            return;
        }

        // 构建通用参数
        std::string commonParam = buildCommonParam(common);
        std::string fullParam;
        std::string endpoint;

        // 处理不同交易类型
        int transType = atoi(response.transId.c_str());
        try {
            switch (transType) {
                case TRANSID_SIGN: {
                    LOG_INFO("处理签到交易");
                    POSTRANS_GT_SIGN_REQUEST request;
                    xpack::json::decode(response.respMsg, request);
                    
                    // 将从request获取的值更新到common中，以便重试时使用
                    common.APP_VERSION = request.APP_VERSION;
                    common.APP_TERMVERSION = request.APP_TERMVERSION;
                    common.ICC_ID = request.ICC_ID;

                    fullParam = commonParam + 
                               "|#|APP_VERSION=" + request.APP_VERSION + 
                               "|#|APP_TERMVERSION=" + request.APP_TERMVERSION +
                               "|#|ICC_ID=" + request.ICC_ID;

                    endpoint = "800502.po";
                    break;
                }
                case TRANSID_CONSUME: {
                    LOG_INFO("处理消费交易");
                    POSTRANS_GT_CONSUME_REQUEST request;
                    xpack::json::decode(response.respMsg, request);

                    fullParam = commonParam + 
                               "|#|THREE_ORDER_NO=" + request.THREE_ORDER_NO + 
                               "|#|PINBLK=" + processPINBLK(request.PINBLK) +
                               "|#|PAY_WAY=" + request.PAY_WAY +
                               "|#|MEDIATYPE=" + request.MEDIATYPE +
                               "|#|CRDNO=" + encryptCardNumber(request.CRDNO, common.POS_TOKEN) +
                               "|#|TRACK2=" + encryptCardNumber(request.TRACK2, common.PINKEY) +
                               "|#|AMOUNT=" + request.AMOUNT +
                               "|#|DCDATA=" + request.DCDATA +
                               "|#|TRAN_TYPE=" + request.TRAN_TYPE +
                               "|#|DEVICE_TYPE=" + request.DEVICE_TYPE +
                               "|#|ENC_RANDOM=" + request.ENC_RANDOM +
                               "|#|TERM_SERIAL_ENCNO=" + request.TERM_SERIAL_ENCNO +
                               "|#|AP_VERSION_NO=" + request.AP_VERSION_NO +
                               "|#|ICNUMBER=" + request.ICNUMBER +
                               "|#|SIGE_TYPE=" + request.SIGE_TYPE +
                               "|#|BUY_PRO_FLAG=" + request.BUY_PRO_FLAG +
                               "|#|REMARK=" + request.REMARK +
                               "|#|SIGN_FLAG=" + request.SIGN_FLAG +
                               "|#|UNI_STRCODE=" + request.UNI_STRCODE;

                    endpoint = "800222.po";
                    break;
                }
                case TRANSID_CANCEL: {
                    LOG_INFO("处理撤销交易");
                    POSTRANS_GT_CANCEL_REQUEST request;
                    xpack::json::decode(response.respMsg, request);

                    fullParam = commonParam + 
                               "|#|THREE_ORDER_NO=" + request.THREE_ORDER_NO + 
                               "|#|ORDER_NO=" + request.ORDER_NO +
                               "|#|CRDNO=" + encryptCardNumber(request.CRDNO, common.POS_TOKEN) +
                               "|#|TRACK2=" + encryptCardNumber(request.TRACK2, common.PINKEY) +
                               "|#|PAY_WAY=" + request.PAY_WAY +
                               "|#|SIGN_FLAG=" + request.SIGN_FLAG;

                    endpoint = "835003.po";
                    break;
                }
                case TRANSID_REFUND: {
                    LOG_INFO("处理退货交易");
                    POSTRANS_GT_REFUND_REQUEST request;
                    xpack::json::decode(response.respMsg, request);

                    fullParam = commonParam + 
                               "|#|THREE_ORDER_NO=" + request.THREE_ORDER_NO + 
                               "|#|ORDER_NO=" + request.ORDER_NO +
                               "|#|REFUND_AMT=" + request.REFUND_AMT +
                               "|#|PAY_WAY=" + request.PAY_WAY +
                               "|#|CRDNO=" + encryptCardNumber(request.CRDNO, common.POS_TOKEN) +
                               "|#|TRACK2=" + encryptCardNumber(request.TRACK2, common.PINKEY) + 
                               "|#|SIGN_FLAG=" + request.SIGN_FLAG + 
                               "|#|DEVICE_TYPE=" + request.DEVICE_TYPE + 
                               "|#|ENC_RANDOM=" + request.ENC_RANDOM + 
                               "|#|TERM_SERIAL_ENCNO=" + request.TERM_SERIAL_ENCNO + 
                               "|#|AP_VERSION_NO=" + request.AP_VERSION_NO + 
                               "|#|DCDATA=" + request.DCDATA + 
                               "|#|ICNUMBER=" + request.ICNUMBER;

                    endpoint = "835001.po";
                    break;
                }
                case TRANSID_QUERY: {
                    LOG_INFO("处理查余交易");
                    POSTRANS_GT_QUERY_REQUEST request;
                    xpack::json::decode(response.respMsg, request);

                    fullParam = commonParam + 
                               "|#|PINBLK=" + processPINBLK(request.PINBLK) + 
                               "|#|PAY_WAY=" + request.PAY_WAY +
                               "|#|MEDIATYPE=" + request.MEDIATYPE +
                               "|#|CRDNO=" + encryptCardNumber(request.CRDNO, common.POS_TOKEN) +
                               "|#|TRACK2=" + encryptCardNumber(request.TRACK2, common.PINKEY) +
                               "|#|DCDATA=" + request.DCDATA + 
                               "|#|DEVICE_TYPE=" + request.DEVICE_TYPE + 
                               "|#|ENC_RANDOM=" + request.ENC_RANDOM + 
                               "|#|TERM_SERIAL_ENCNO=" + request.TERM_SERIAL_ENCNO + 
                               "|#|AP_VERSION_NO=" + request.AP_VERSION_NO + 
                               "|#|ICNUMBER=" + request.ICNUMBER;

                    endpoint = "800221.po";
                    break;
                }
                case TRANSID_REPRINT: {
                    LOG_INFO("处理重打印交易");
                    POSTRANS_GT_REPRINT_REQUEST request;
                    xpack::json::decode(response.respMsg, request);

                    fullParam = commonParam + 
                               "|#|OPER_TYPE=" + request.OPER_TYPE + 
                               "|#|TXNLOGID=" + request.TXNLOGID;

                    endpoint = "800703.po";
                    break;
                }
                case TRANSID_DOWNLOADKEY: {
                    LOG_INFO("处理密钥下载");

                    fullParam = commonParam;

                    endpoint = "800503.po";
                    break;
                }
                case TRANSID_ACTIVE: {
                    LOG_INFO("处理绑定激活");

                    fullParam = commonParam + 
                                "|#|ACTIVE_CODE=" + response.respMsg;

                    endpoint = "800503.po";
                    break;
                }
                default:
                    LOG_ERROR("未知交易类型: %s", response.transId.c_str());
                    std::string errorMsg = "{\"RSPCOD\":\"XX\",\"RSPMSG\":\"未知交易类型\"}";
                    copyToSendBuffer(errorMsg, sendbuf, sendLen);
                    return;
            }
        } catch (const std::exception& e) {
            LOG_ERROR("解析请求参数失败: %s", e.what());
            std::string errorMsg = "{\"RSPCOD\":\"XX\",\"RSPMSG\":\"解析请求参数失败: " + std::string(e.what()) + "\"}";
            copyToSendBuffer(errorMsg, sendbuf, sendLen);
            return;
        }

        // 处理交易
        if (!processTransaction(fullParam, endpoint, common, sendbuf, sendLen)) {
            std::string errorMsg = "{\"RSPCOD\":\"XX\",\"RSPMSG\":\"处理交易失败\"}";
            copyToSendBuffer(errorMsg, sendbuf, sendLen);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("处理异常: %s", e.what());
        std::string errorMsg = "{\"RSPCOD\":\"XX\",\"RSPMSG\":\"系统处理异常:" + std::string(e.what()) + "\"}";
        copyToSendBuffer(errorMsg, sendbuf, sendLen);
    } catch (...) {
        LOG_ERROR("处理时发生未知异常");
        std::string errorMsg = "{\"RSPCOD\":\"XX\",\"RSPMSG\":\"系统处理时发生未知异常\"}";
        copyToSendBuffer(errorMsg, sendbuf, sendLen);
    }
}

void action(char* recvbuf, int recvLen, char* sendbuf, int* sendLen)
{

    std::string jsonStr(recvbuf + 2, recvLen - 2);
    LOG_INFO("请求数据: %s", jsonStr.c_str());

    try {
        POSTRANS_RESPONSE response;
        xpack::json::decode(jsonStr, response);
        processGT(recvbuf + 2, recvLen - 2, sendbuf, sendLen, response);
    } catch (const std::exception& e) {
        std::string errorMsg = "解析错误: ";
        errorMsg += e.what();
        memcpy(sendbuf, errorMsg.c_str(), errorMsg.length());
        *sendLen = errorMsg.length();
        LOG_ERROR("解析错误: %s", e.what());
    }
}