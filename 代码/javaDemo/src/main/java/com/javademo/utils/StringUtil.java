package com.javademo.utils;

import java.math.RoundingMode;
import java.text.DecimalFormat;
import java.text.NumberFormat;

public class StringUtil
{
  public static String byteToHex(byte[] paramArrayOfByte)
  {
    StringBuilder localStringBuilder = new StringBuilder("");
    for (int i = 0; i < paramArrayOfByte.length; i++)
    {
      String str = Integer.toHexString(paramArrayOfByte[i] & 0xFF);
      Object localObject = str;
      if (str.length() == 1)
      {
        localObject = new StringBuilder();
        ((StringBuilder)localObject).append("0");
        ((StringBuilder)localObject).append(str);
        localObject = ((StringBuilder)localObject).toString();
      }
      localStringBuilder.append((String)localObject);
    }
    return localStringBuilder.toString().trim();
  }

  public static String formatBankCard(String paramString)
  {
    return paramString.replaceAll("\\S{4}(?!$)", "$0 ");
  }

  public static String formatSixNumber(int paramInt)
  {
    if (paramInt >= 0)
    {
      DecimalFormat localDecimalFormat = (DecimalFormat)NumberFormat.getInstance();
      localDecimalFormat.setRoundingMode(RoundingMode.DOWN);
      localDecimalFormat.applyPattern("000000");
      return localDecimalFormat.format(paramInt);
    }
    return "";
  }

  public static byte[] hexToByte(String paramString)
  {
    int i = paramString.length() / 2;
    byte[] arrayOfByte = new byte[i];
    for (int j = 0; j < i; j++)
    {
      int k = j * 2;
      int m = k + 1;
      StringBuilder localStringBuilder = new StringBuilder();
      localStringBuilder.append("0x");
      localStringBuilder.append(paramString.substring(k, m));
      localStringBuilder.append(paramString.substring(m, m + 1));
      arrayOfByte[j] = Byte.valueOf((byte)Integer.decode(localStringBuilder.toString()).intValue()).byteValue();
    }
    return arrayOfByte;
  }

  public static String removeSpace(String paramString)
  {
    return paramString.replaceAll("\\s*", "");
  }
}

/* Location:           D:\MyGit\YLScanPay\文档\classes.jar
 * Qualified Name:     com.landi.unionCodePay.utils.StringUtil
 * JD-Core Version:    0.6.2
 */