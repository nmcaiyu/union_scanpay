package com.javademo;

import cn.hutool.core.date.DateUnit;
import cn.hutool.core.util.RandomUtil;
import cn.hutool.http.HttpRequest;
import com.javademo.bean.NetWorkRequestActivation;
import com.javademo.bean.NetWorkRequestTrans;
import com.javademo.utils.StringUtil;
import com.javademo.utils.XmlUtil;

import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLSocketFactory;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Field;
import java.net.URL;
import java.net.URLEncoder;
import java.security.*;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.*;

public class main {

    public static final String deviceInfo = "EPOS";
    public static final String vn = "000003";
    public static final String sn = "222333";
    public static KeyPair localKey;

    public static KeyPair initKey()
            throws Exception
    {
        KeyPairGenerator localKeyPairGenerator = KeyPairGenerator.getInstance("RSA");
        localKeyPairGenerator.initialize(2048);
        return localKeyPairGenerator.generateKeyPair();
    }

    public static void main(String[] args) throws Exception {
        localKey = initKey();
        System.out.println("========== 私钥 ==========");
        System.out.println(Base64.getEncoder().encodeToString(localKey.getPrivate().getEncoded()));
        System.out.println("========== 公钥 ==========");
        System.out.println(Base64.getEncoder().encodeToString(localKey.getPublic().getEncoded()));

        while (true){
            System.out.println("输入交易步骤");
            String data = new Scanner(System.in).nextLine();
            switch (data){
                case "1":
                    System.out.println("开始绑定设备");
                    bindDevice();
                    break;
                case "2":
                    System.out.println("开始查询设备");
                    queryDevice();
                    break;
                case "3":
                    System.out.println("开始查询交易订单");
                    queryOrder();
                    break;
                case "0":
                    return;
            }
        }
    }

    private static SortedMap<String, String> getKeyValue(Object paramObject)
    {
        TreeMap localTreeMap = new TreeMap();
        Object localObject = paramObject.getClass();
        ArrayList localArrayList = new ArrayList();
        while (localObject != Object.class)
        {
            localArrayList.addAll(Arrays.asList(((Class)localObject).getDeclaredFields()));
            localObject = ((Class)localObject).getSuperclass();
        }
        for (int i = 0; i < localArrayList.size(); i++)
        {
            localObject = (Field)localArrayList.get(i);
            ((Field)localObject).setAccessible(true);
            try
            {
                if ((((Field)localObject).get(paramObject) != null) && (!((Field)localObject).getName().equals("serialVersionUID"))) {
                    localTreeMap.put(((Field)localObject).getName(), ((Field)localObject).get(paramObject).toString());
                }
            }
            catch (IllegalAccessException localIllegalAccessException)
            {
                localIllegalAccessException.printStackTrace();
            }
            catch (IllegalArgumentException localIllegalArgumentException)
            {
                localIllegalArgumentException.printStackTrace();
            }
            ((Field)localObject).setAccessible(false);
        }
        return localTreeMap;
    }
    private static String getSbk(Object paramObject)
    {
        StringBuffer localStringBuffer = new StringBuffer();
        Iterator localIterator = getKeyValue(paramObject).entrySet().iterator();
        while (localIterator.hasNext())
        {
            Object localObject = (Map.Entry)localIterator.next();
            paramObject = (String)((Map.Entry)localObject).getKey();
            localObject = (String)((Map.Entry)localObject).getValue();
            if ((localObject != null) && (!"".equals(localObject)) && (!"sign".equals(paramObject)) && (!"key".equals(paramObject)))
            {
                StringBuilder localStringBuilder = new StringBuilder();
                localStringBuilder.append(paramObject);
                localStringBuilder.append("=");
                localStringBuilder.append((String)localObject);
                localStringBuilder.append("&");
                localStringBuffer.append(localStringBuilder.toString());
            }
        }
        System.out.println(localStringBuffer);
        return localStringBuffer.substring(0, localStringBuffer.length() - 1);
    }

    public static byte[] getSHA256(byte[] paramArrayOfByte)
    {
        try
        {
            MessageDigest localMessageDigest = MessageDigest.getInstance("SHA-256");
            localMessageDigest.update(paramArrayOfByte);
            paramArrayOfByte = localMessageDigest.digest();
        }
        catch (NoSuchAlgorithmException e)
        {
            e.printStackTrace();
            paramArrayOfByte = null;
        }
        return paramArrayOfByte;
    }
    public static byte[] sign(byte[] paramArrayOfByte, String paramString)
            throws Exception
    {
        PrivateKey key;
        key = KeyFactory.getInstance("RSA").generatePrivate(new PKCS8EncodedKeySpec(Base64.getDecoder().decode(paramString)));
        Signature localSignature = Signature.getInstance("SHA256WithRSA");
        localSignature.initSign(key);
        localSignature.update(paramArrayOfByte);
        return localSignature.sign();
    }

    private static void queryOrder() throws Exception {
        NetWorkRequestTrans requestTrans = new NetWorkRequestTrans();
        requestTrans.setService("unified.trade.query");
        requestTrans.setOut_trade_no("20220725000001");
        requestTrans.setDevice_info(deviceInfo);
        requestTrans.setSn_id(vn + sn);
        requestTrans.setSign_type("RSA2");
        requestTrans.setMch_id("QRA2015762207PU");
        requestTrans.setMch_create_ip("127.0.0.1");
        requestTrans.setNonce_str(RandomUtil.randomString(10));

        byte[] signData = StringUtil.byteToHex(getSHA256(getSbk(requestTrans).getBytes())).toLowerCase().getBytes();
        System.out.println(StringUtil.byteToHex(signData));
        requestTrans.setSign(Base64.getEncoder().encodeToString(sign(signData, Base64.getEncoder().encodeToString(localKey.getPrivate().getEncoded()))));


        String data = XmlUtil.beanToXml(requestTrans, true);
        System.out.println(data);

        String result2 = HttpRequest.post("https://qra.95516.com/pay/security/gateway")
                .body(data)
                .execute().body();
        System.out.println(result2);
    }

    private static void queryDevice() {
        NetWorkRequestActivation requestActivation = new NetWorkRequestActivation();
        requestActivation.setSN_ID(vn + sn);
        requestActivation.setVN(vn);
        requestActivation.setDEVICE_INFO(deviceInfo);
        System.out.println("绑定成功后输入token进行查询");
        String token = new Scanner(System.in).nextLine();
        requestActivation.setSN_QR_INIT_TOKEN(token);

        String data = XmlUtil.beanToXml(requestActivation, true);
        System.out.println(data);

        String result2 = HttpRequest.post("https://ac.95516.com/spay/device/epos/queryinit")
                .body(data)
                .execute().body();
        System.out.println(result2);
    }

    private static void bindDevice() {
        NetWorkRequestActivation requestActivation = new NetWorkRequestActivation();
        requestActivation.setSN_ID(vn + sn);
        requestActivation.setVN(vn);
        requestActivation.setDEVICE_INFO(deviceInfo);


        requestActivation.setDEVICE_PUB_KEY(Base64.getEncoder().encodeToString(localKey.getPublic().getEncoded()));

        String data = XmlUtil.beanToXml(requestActivation, true);
        System.out.println(data);

        String result2 = HttpRequest.post("https://ac.95516.com/spay/device/epos/getinit")
                .body(data)
                .execute().body();
        System.out.println(result2);
    }

}
