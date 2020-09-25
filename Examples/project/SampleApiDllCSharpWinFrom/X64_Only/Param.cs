using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SampleApiDllCSharpWinForm
{
    // todo: write the parameter property page automaticly. -------------------

    //XProps xps = new XProps();
    //XProp xprop = new XProp();
    //xprop.Name = "姓名";
    //xprop.Value = "某人";
    //xprop.Category = "人类";
    //xprop.Description = "姓甚名谁";
    //xprop.ProType = typeof(String);
    //xprop.ReadOnly = true;
    //xps.Add(xprop);

    //xprop = new XProp();
    //xprop.Category = "人类";
    //xprop.Name = "年龄";
    //xprop.ProType = typeof(int);
    //xprop.Value = "2";
    //xprop.Description = "多大年纪";
    //xprop.ReadOnly = false;
    //xps.Add(xprop);

    //xprop = new XProp();
    //xprop.Category = "人类";
    //xprop.Name = "性别";
    //xprop.Value = 1;
    //xprop.ReadOnly = false;
    //xprop.ProType = typeof(MyComboItemConvert);
    //xprop.Converter = new MyComboItemConvert("M,F");
    //xprop.Description = "性别是男是女";
    //xps.Add(xprop);

    //xprop = new XProp();
    //xprop.Category = "人类";
    //xprop.ReadOnly = false;
    //xprop.Name = "国籍";
    //xprop.Value = 1;
    //xprop.ProType = typeof(MyComboItemConvert);
    //xprop.Converter = new MyComboItemConvert("中,英,美,法");
    //xprop.Description = "国籍";
    //xps.Add(xprop);


    ////m_PropsParam.Add(newXpro);

    //propertyGrid1.SelectedObject = xps;

    // http://blog.sina.com.cn/s/blog_6f14b7010101b91b.html
    //单条属性类
    public class XProp
    {
        //private string theId = "";          //属性Id
        private string      m_Category     = "";       //属性所属类别
        private string      m_Name         = "";       //属性名称
        private bool        m_ReadOnly     = false;    //属性的只读性，true为只读
        private string      m_Description  = "";       //属性的描述内容
        private object      m_Value        = null;     //值: int for the one with options, string for the numerical.
        private System.Type m_Type         = null;     //类型
        private bool        m_Browsable    = true;     //显示或隐藏，true为显示
        TypeConverter       m_Converter    = null;     //类型转换
        private bool        m_Update       = false;    //
        private String[]    m_Options      = new String[0];// 候选项


        //public string Id
        //{
        //    get { return m_Id; }
        //    set { m_Id = value; }
        //}
        public string Category
        {
            get { return m_Category; }
            set { m_Category = value; }
        }
        public bool ReadOnly
        {
            get { return m_ReadOnly; }
            set { m_ReadOnly = value; }
        }
        public string Name
        {
            get { return this.m_Name; }
            set { this.m_Name = value; }
        }
        public object Value
        {
            get { return this.m_Value; }
            set { this.m_Value = value; }
        }
        public string Description
        {
            get { return m_Description; }
            set { m_Description = value; }
        }
        public System.Type ProType
        {
            get { return m_Type; }
            set { m_Type = value; }
        }
        public bool Browsable
        {
            get { return m_Browsable; }
            set { m_Browsable = value; }
        }
        public virtual TypeConverter Converter
        {
            get { return m_Converter; }
            set { m_Converter = value; }
        }
        public virtual bool Update
        {
            get { return m_Update; }
            set { m_Update = value; }
        }
        public virtual String[] Options
        {
            get { return m_Options; }
            set { m_Options = value; }
        }

    }

    public class XProps : List<XProp>, ICustomTypeDescriptor
    {
        #region ICustomTypeDescriptor 成员

        public AttributeCollection GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
        }

        public string GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }

        public string GetComponentName()
        {
            return TypeDescriptor.GetComponentName(this, true);
        }

        public TypeConverter GetConverter()
        {
            return TypeDescriptor.GetConverter(this, true);
        }

        public EventDescriptor GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(this, true);
        }

        public PropertyDescriptor GetDefaultProperty()
        {
            return TypeDescriptor.GetDefaultProperty(this, true);
        }

        public object GetEditor(System.Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        public EventDescriptorCollection GetEvents(System.Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(this, attributes, true);
        }

        public EventDescriptorCollection GetEvents()
        {
            return TypeDescriptor.GetEvents(this, true);
        }

        public PropertyDescriptorCollection GetProperties(System.Attribute[] attributes)
        {
            ArrayList props = new ArrayList();
            for (int i = 0; i < this.Count; i++)
            {  //判断属性是否显示
                if (this[i].Browsable == true)
                {
                    XPropDescriptor psd = new XPropDescriptor(this[i], attributes);
                    props.Add(psd);
                }
            }
            PropertyDescriptor[] propArray = (PropertyDescriptor[])props.ToArray(typeof(PropertyDescriptor));
            return new PropertyDescriptorCollection(propArray);
        }

        public PropertyDescriptorCollection GetProperties()
        {
            return TypeDescriptor.GetProperties(this, true);
        }

        public object GetPropertyOwner(PropertyDescriptor pd)
        {
            return this;
        }

        #endregion

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < this.Count; i++)
            {
                sb.Append("[" + i + "] " + this[i].ToString() + System.Environment.NewLine);
            }
            return sb.ToString();
        }
    }

    public class XPropDescriptor : PropertyDescriptor
    {
        XProp theProp;
        public XPropDescriptor(XProp prop, Attribute[] attrs) : base(prop.Name, attrs)
        {
            theProp = prop;
        }

        public override bool CanResetValue(object component)
        {
            return false;
        }
        public override string Category
        {
            get { return theProp.Category; }
        }

        public override string Description
        {
            get { return theProp.Description; }
        }

        public override TypeConverter Converter
        {
            get { return theProp.Converter; }
        }

        public override System.Type ComponentType
        {
            get { return this.GetType(); }
        }

        public override object GetValue(object component)
        {
            return theProp.Value;
        }

        public override bool IsReadOnly
        {
            get { return theProp.ReadOnly; }
        }

        public override System.Type PropertyType
        {
            get { return theProp.ProType; }
        }

        public override void ResetValue(object component)
        {
        }

        public override void SetValue(object component, object value)
        {
            theProp.Update = theProp.Value != value;
            theProp.Value = value;
        }

        public override bool ShouldSerializeValue(object component)
        {
            return false;
        }
    }

    public abstract class ComboBoxItemTypeConvert : TypeConverter
    {
        public Hashtable myhash = null;
        public ComboBoxItemTypeConvert()
        {
            myhash = new Hashtable();
            GetConvertHash();
        }
        public abstract void GetConvertHash();

        //是否支持选择列表的编辑
        public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
        {
            return true;
        }
        //重写combobox的选择列表
        public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            int[] ids = new int[myhash.Values.Count];
            int i = 0;
            foreach (DictionaryEntry myDE in myhash)
            {
                ids[i++] = (int)(myDE.Key);
            }
            return new StandardValuesCollection(ids);
        }
        //判断转换器是否可以工作
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(string))
            {
                return true;
            }
            return base.CanConvertFrom(context, sourceType);
        }
        //重写转换器，将选项列表（即下拉菜单）中的值转换到该类型的值
        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object obj)
        {
            if (obj is string)
            {
                foreach (DictionaryEntry myDE in myhash)
                {
                    if (myDE.Value.Equals((obj.ToString())))
                        return myDE.Key;
                }
            }
            return base.ConvertFrom(context, culture, obj);
        }
        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(string))
            {
                return true;
            }
            return base.CanConvertTo(context, destinationType);
        }
        //重写转换器将该类型的值转换到选择列表中
        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object obj, Type destinationType)
        {
            if (destinationType == typeof(string))
            {
                foreach (DictionaryEntry myDE in myhash)
                {
                    if (myDE.Key.Equals(obj))
                        return myDE.Value.ToString();
                }
                return "";
            }
            return base.ConvertTo(context, culture, obj, destinationType);
        }
        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
        {
            return false;
        }
    }
    
    //重写下拉菜单，在这里实现定义下拉菜单内的项
    public class MyComboItemConvert : ComboBoxItemTypeConvert
    {
        private Hashtable hash;
        public override void GetConvertHash()
        {
            try
            {
                myhash = hash;
            }
            catch
            {
                throw new NotImplementedException();
            }
        }
        public MyComboItemConvert(string str)
        {
            hash = new Hashtable();
            string[] stest = str.Split(',');
            for (int i = 0; i < stest.Length; i++)
            {
                hash.Add(i, stest[i]);
            }
            GetConvertHash();
            value = 0;
        }
        public int value { get; set; }
        public MyComboItemConvert(string str, int s)
        {
            hash = new Hashtable();
            string[] stest = str.Split(',');
            for (int i = 0; i < stest.Length; i++)
            {
                hash.Add(i, stest[i]);
            }
            GetConvertHash();
            value = s;
        }
    }

}
