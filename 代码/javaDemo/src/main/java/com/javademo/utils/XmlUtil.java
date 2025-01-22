package com.javademo.utils;

import com.thoughtworks.xstream.XStream;
import com.thoughtworks.xstream.core.util.QuickWriter;
import com.thoughtworks.xstream.io.HierarchicalStreamWriter;
import com.thoughtworks.xstream.io.naming.NameCoder;
import com.thoughtworks.xstream.io.xml.DomDriver;
import com.thoughtworks.xstream.io.xml.PrettyPrintWriter;
import com.thoughtworks.xstream.io.xml.XmlFriendlyNameCoder;
import java.io.Writer;

public class XmlUtil
{
  private static String PREFIX_CDATA = "<![CDATA[";
  private static String SUFFIX_CDATA = "]]>";
  private static final boolean isAddCDATA = true;
  private static XStream xs = getInstance();

  public static String beanToXml(Object paramObject, boolean paramBoolean)
  {
    xs = getInstance();
    XStream localXStream = xs;
    String str;
    if (paramBoolean) {
      str = "XML";
    } else {
      str = "xml";
    }
    localXStream.alias(str, paramObject.getClass());
    return StringUtil.removeSpace(xs.toXML(paramObject).replace("__", "_"));
  }

  private static XStream getInstance()
  {
    xs = new XStream(new DomDriver("utf-8", new XmlFriendlyNameCoder("_-", "_"))
    {
      @Override
      public HierarchicalStreamWriter createWriter(Writer paramAnonymousWriter)
      {
        return new PrettyPrintWriter(paramAnonymousWriter)
        {
          @Override
          public void startNode(String paramAnonymous2String)
          {
            super.startNode(paramAnonymous2String);
          }

          @Override
          protected void writeText(QuickWriter paramAnonymous2QuickWriter, String paramAnonymous2String)
          {
            StringBuilder localStringBuilder = new StringBuilder();
            localStringBuilder.append(XmlUtil.PREFIX_CDATA);
            localStringBuilder.append(paramAnonymous2String);
            localStringBuilder.append(XmlUtil.SUFFIX_CDATA);
            paramAnonymous2QuickWriter.write(localStringBuilder.toString());
          }
        };
      }
    });
    return xs;
  }

  public static <T> T xmlToObjFromClass(String paramString, Class<T> paramClass, boolean paramBoolean)
  {
    xs = getInstance();
    xs.ignoreUnknownElements();
    XStream localXStream = xs;
    String str;
    if (paramBoolean) {
      str = "XML";
    } else {
      str = "xml";
    }
    localXStream.alias(str, paramClass);
    return (T) xs.fromXML(paramString);
  }
}

/* Location:           D:\MyGit\YLScanPay\文档\classes.jar
 * Qualified Name:     com.landi.unionCodePay.utils.XmlUtil
 * JD-Core Version:    0.6.2
 */