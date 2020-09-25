// 单元测试 预编译宏
#define UnitTest

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Collections;
using SICK3D;


namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {
        
        #region // Global variables. 全局变量

        public GenICam3D    m_r3s       = new GenICam3D("", "", false, true);
        public String       m_ip        = "";                                   // IP of Current selected Ranger3.

        public Dictionary<String, Ranger3>   
                            m_list_r3   = new Dictionary<String, Ranger3>();    // Key=Id, Value=CSR3

        public DataNames    m_GrabMode  = DataNames.SEN;                  
        public System.Timers.Timer 
                            myTimer     = new System.Timers.Timer();

        int                 m_reqStop   = 2;                                     // 0_req, 1_finish, 2_init 
        CSImgT              m_lastImg   = new CSImgT();
        XProps              m_PropsParam= new XProps();

        private readonly Object m_imgLocker = new object(); 

        #endregion


        public frmMain()
        {
            InitializeComponent();
            myTimer.Interval = 10;
            myTimer.Elapsed += GrabImageTimer;

#if UnitTest
            unitTestToolStripMenuItem.Enabled = true;
#else
            unitTestToolStripMenuItem.Enabled = fasle;
#endif
        }

        private void GrabImageTimer(object sender, System.Timers.ElapsedEventArgs e)
        {
            // 响应停止请求
            if (m_reqStop == 0)
            {
                m_reqStop = 1;
                myTimer.Enabled = false;
            }

            if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED && R3(m_ip).isGrabbed())
            {
                CSImgT imgTable = new CSImgT();
                if (R3(m_ip).getImageData(imgTable) == CAM_STATUS.All_OK)
                {

                    Bitmap imgGray;

                    // 读取图片 convert image to show
                    var mode = GetGrabMode();
                    if (!imgTable.has(mode))
                        return;

                    switch (mode)
                    {
                        case DataNames.SEN:
                            imgGray = BuiltGrayBitmap(
                                imgTable.getDataByte(mode),
                                imgTable.getS_cols(),
                                imgTable.getS_rows()
                                );
                            break;
                        case DataNames.REF:
                        case DataNames.SCA:
                        case DataNames.REF_CAL:
                        case DataNames.SCA_CAL:
                        case DataNames.REF2:
                        case DataNames.SCA2:
                        case DataNames.REF2_CAL:
                        case DataNames.SCA2_CAL:
                            imgGray = BuiltGrayBitmap(
                                imgTable.getDataByte(mode),
                                imgTable.getR_cols(),
                                imgTable.getR_rows()
                                );
                            break;
                        case DataNames.RAN:
                        case DataNames.RAN2:
                            imgGray = BuiltGrayBitmap(
                                imgTable.getDataWord(mode),
                                imgTable.getR_cols(),
                                imgTable.getR_rows()
                                );
                            break;
                        case DataNames.RAN_CAL:
                        case DataNames.RAN2_CAL:
                            imgGray = BuiltGrayBitmap(
                                imgTable.getDataFloat(mode),
                                imgTable.getR_cols(),
                                imgTable.getR_rows()
                                );
                            break;
                        default:
                            imgTable.Dispose();
                            return;
                    }

                    // 显示图片 Show Image
                    // Code below will cost a lot of memory.  
                    // This is a side effect of C# gc.  
#if true                        
                    System.Drawing.Image imgShow = System.Drawing.Image.FromHbitmap(imgGray.GetHbitmap());
                    pictureBox1.BackgroundImage = imgShow.GetThumbnailImage(pictureBox1.Size.Width, pictureBox1.Size.Height, null, IntPtr.Zero);
                    imgShow.Dispose();
                    imgGray.Dispose();
                    imgShow = null;
                    imgGray = null;
                    GC.Collect();
                    GC.WaitForPendingFinalizers();
#endif

                    lock (m_imgLocker)
                    {
                        m_lastImg._deepCopy(imgTable);
                    }

                    // 响应停止请求
                    if (m_reqStop == 0)
                    {
                        m_reqStop = 1;
                        myTimer.Enabled = false;
                    }

                    // show ID
                    wFlog(imgTable.get_ID().ToString());

                }
                imgTable.Dispose();
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            FindCamera();
            if (R3(m_ip) != null && R3(m_ip).isReady() == CAM_STATUS.All_OK)
            {
                toolStripComboBoxMode.SelectedIndex = 1;
                //m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
            }
        }


        private void clearPropertyGrid()
        {
            m_PropsParam.Clear();
            propertyGrid1.SelectedObject = m_PropsParam;
            //MessageBox.Show("Clear propertyGrid1");
        }
        private void addPropertyGrid(String cate, String name, Type type, object value, String desc, String[] conv, bool readOnly)
        {
            // write the parameter property page automaticly. 
            XProp xprop = new XProp();
            xprop.Category = cate;
            xprop.Name = name;
            xprop.ProType = type; // typeof(int); typeof(MyComboItemConvert);
            xprop.Value = value;
            xprop.Description = desc;
            xprop.Options = conv;
            if (conv.Length > 0)
            {
                String optStr = "";
                for (int i = 0; i < conv.Length; ++i)
                {
                    optStr = optStr + conv[i];
                    if (i + 1 < conv.Length)
                        optStr = optStr + ",";
                }

                xprop.Converter = new MyComboItemConvert(optStr);

            }
            xprop.ReadOnly = readOnly;

            m_PropsParam.Add(xprop);
            propertyGrid1.SelectedObject = m_PropsParam;
        }
        private void updateParamPropertyGrid()
        {
            clearPropertyGrid();
            if (R3(m_ip).updateParamtersInfo())
            {
                String[] cates = R3(m_ip).paramCategoryList();
                foreach (var cat in cates)
                {
                    var paras = R3(m_ip).paramFullNameOfCategory(cat);

                    foreach (var par in paras)
                    {
                        String cValue = R3(m_ip).paramCurrentValue(par);
                        int cValueId = 0; // for the numerical parameter only
                        String[] opts = R3(m_ip).paramOptionalValues(par, ref cValueId);
                        if (opts.Length == 0)
                        {
                            addPropertyGrid(cat, par, typeof(String), cValue, "", opts, false);
                        }
                        else
                        {
                            addPropertyGrid(cat, par, typeof(MyComboItemConvert), cValueId, "", opts, false);
                        }

                    }
                }
            }
        }


        private void toolStripButtonFindCamera_Click(object sender, EventArgs e)
        {
            {
                treeView1.Nodes.Clear();
                m_list_r3.Clear();
            }
            FindCamera();
            if (/*R3(m_ip) != null && */R3(m_ip).isReady() == CAM_STATUS.All_OK)
            {
                toolStripComboBoxMode.SelectedIndex = Convert.ToInt32(DataNames.SEN);
                //m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
            }
            else
                MessageBox.Show("没有找到相机！ No Cameras found!");
        }

        private void toolStripButtonConnection_Click(object sender, EventArgs e)
        {
            Connection();
        }

        private void toolStripButtonDisconnection_Click(object sender, EventArgs e)
        {
            Disconnection();
        }

        private void toolStripButtonStart_Click(object sender, EventArgs e)
        {
            Start(); 
        }

        private void toolStripButtonStop_Click(object sender, EventArgs e)
        {
            Stop(); 
        }


        // if return false, caller need to keep toolStripComboBoxMode.SelectedIndex avaliable.
        // if return true, toolStripComboBoxMode.SelectedIndex is avalibale. But it may be changed in this function.
        private bool checkGrabMode()
        {
            if (!(R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_CONNECTED || R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STOPPED))
                return false;

            // 检查 Grab Mode 和参数设置是否匹配。以参数设置为标准，应该调整 Grab Mode 以适应当前的参数设置。
            // Check Grab Mode and parameters. The Grab Mode shoud be modified to match current parameters.
            var scanType = R3(m_ip).getParameterValue("DeviceScanType");
            if (scanType == "Areascan") // senseor
            {
                var mode = GetGrabMode();
                if (mode != DataNames.SEN)
                {
                    MessageBox.Show("检测到 \"Grab Mode\" 是 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + "与参数 <DeviceScanType = Areascan> 不匹配，关闭窗口后将会强制设置 \"Grab Mode\" 为 [Sensor]。 \n"
                        + "请检查：\n"
                        + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                        + "2. 若想起用 \"Grab Mode\" 中的设置，请设置 <DeviceScanType = Linescan3D> \n\n\n"
                        + "\"Grab Mode\" is [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + " that mismatch the parameter <DeviceScanType = Areascan>. After closing window it will set \"Grab Mode\" as [Sensor] automaticly. "
                        + "Please check:\n"
                        + "1. Have updated parameters? Please click <Set Param> to enabel setting. \n"
                        + "2. If you want to keep the setting of \"Grab Mode\", please set <DeviceScanType = Linescan3D> \n\n\n"
                        , "WARNING");
                    toolStripComboBoxMode.SelectedIndex = (int)DataNames.SEN;
                    //m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
                    return true;
                }
            }
            else if (scanType == "Linescan3D") // range
            {
                var mode = GetGrabMode();

                // 检查 Grab Mode 不能是 2D 模式
                if (mode == DataNames.SEN)
                {
                    MessageBox.Show("检测到 \"Grab Mode\" 是 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + "与参数 <DeviceScanType = Linescan3D> 不匹配，关闭窗口后将会强制设置 \"Grab Mode\" 为 [Range]。 \n"
                        + "请检查：\n"
                        + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n\n\n"
                        + "\"Grab Mode\" is [" + toolStripComboBoxMode.SelectedItem.ToString() + "] "
                        + " that mismatch the parameter <DeviceScanType = Linescan3D>. After closing window it will set \"Grab Mode\" as [Range] automaticly. " + "Please check:\n"
                        + "Please check:\n"
                        + "1. Have updated parameters? Please click <Set Param> to enabel setting."
                        , "WARNING");
                    toolStripComboBoxMode.SelectedIndex = (int)DataNames.RAN;
                    //m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
                    return true;
                }
                else
                {
                    // Grab Mode 是 3D 模式

                    // 检查标定文件
                    // Check calibraton file
                    if (mode == DataNames.REF_CAL || mode == DataNames.RAN_CAL || mode == DataNames.REF2_CAL || mode == DataNames.RAN2_CAL)
                    {
                        if (R3(m_ip).getCalibraPath() == "")
                        {
                            MessageBox.Show("没找到标定文件，请手动导入！ \n\n\nNot Calibration File found!!!", "ERROR");
                            return false;
                        }
                    }

                    // Refelctance1 使能设置检查
                    if (mode == DataNames.REF || mode == DataNames.REF_CAL)
                    {
                        if ("1" != R3(m_ip).getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1"))
                        {
                            MessageBox.Show("检测到 \"Grab Mode\" 设置值 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] 不匹配当前参数设置。\n"
                                + "该 Grab Mode 需要相机使能 <Reflectance (激光强度图)> 。\n"
                                + "请检查：\n"
                                + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                                + "2. 请将 <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1> 设置为 [1] \n"
                                + "3. 请将 \"Grab Mode\" 设置 [Range] \n\n\n"
                                + "\"Grab Mode\" = [" + toolStripComboBoxMode.SelectedItem.ToString() + "] mismatch current parameter setting. \n"
                                + "It need enable <Reflectance> \n"
                                + "Please check:\n"
                                + "1. Have updated parameters? Please click <Set Param> to enabel setting.\n"
                                + "2. Please set <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1> = [1]\n"
                                + "3. Please set \"Grab Mode\" as [Range]. \n\n\n"
                                , "ERROR");
                            return false;
                        }
                    }

                    // Range2 使能设置检查
                    if (mode == DataNames.RAN2 || mode == DataNames.RAN2_CAL)
                    {
                        var ran2 = R3(m_ip).getParameterValue("ComponentEnable_ComponentSelector_Range_RegionSelector_Scan3dExtraction2");
                        if (ran2 == "0")
                        {
                            MessageBox.Show("检测到 \"Grab Mode\" 设置值 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] 不匹配当前参数设置。\n"
                                + "该 Grab Mode 需要相机使能【Extra 算法 Scan3dExtraction2】中的 <Range (激光强度图)> 。\n"
                                + "请检查：\n"
                                + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                                + "2. 请将 <ComponentEnable_ComponentSelector_Range_RegionSelector_Scan3dExtraction2> 设置为 [1] \n"
                                + "3. 请将 \"Grab Mode\" 设置 [Range] \n\n\n"
                                + "\"Grab Mode\" = [" + toolStripComboBoxMode.SelectedItem.ToString() + "] mismatch current parameter setting. \n"
                                + "It need enable <Range of Scan3dExtraction2>\n"
                                + "Please check:\n"
                                + "1. Have updated parameters? Please click <Set Param> to enabel setting.\n"
                                + "2. Please set <ComponentEnable_ComponentSelector_Range_RegionSelector_Scan3dExtraction2> = [1]\n"
                                + "3. Please set \"Grab Mode\" as [Range]. \n\n\n"
                                , "ERROR");
                            return false;
                        }
                        else if (ran2 == "")
                        {
                            MessageBox.Show("不支持当前设置。 \n\n\nNot Support Grab Mode yet!", "ERROR");
                            return false;
                        }

                    }

                    // Refelctance2 使能设置检查
                    if (mode == DataNames.REF2 || mode == DataNames.REF2_CAL)
                    {
                        var ref2 = R3(m_ip).getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2");
                        if ("0" == ref2)
                        {
                            MessageBox.Show("检测到 \"Grab Mode\" 设置值 [" + toolStripComboBoxMode.SelectedItem.ToString() + "] 不匹配当前参数设置。\n"
                                + "该 Grab Mode 需要相机使能【Extra 算法 Scan3dExtraction2】中的 <Reflectance (激光强度图)> 。\n"
                                + "请检查：\n"
                                + "1. 是否是未更新参数？请点击工具栏 <Set Param> 以使能设置！\n"
                                + "2. 请将 <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2> 设置为 [1] \n"
                                + "3. 请将 \"Grab Mode\" 设置 [Range] \n\n\n"
                                + "\"Grab Mode\" = [" + toolStripComboBoxMode.SelectedItem.ToString() + "] mismatch current parameter setting. \n"
                                + "It need enable <Reflectance of Scan3dExtraction2>\n"
                                + "Please check:\n"
                                + "1. Have updated parameters? Please click <Set Param> to enabel setting.\n"
                                + "2. Please set <ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction2> = [1]\n"
                                + "3. Please set \"Grab Mode\" as [Range]. \n\n\n"
                                , "ERROR");
                            return false;
                        }
                        else if (ref2 == "")
                        {
                            MessageBox.Show("不支持当前设置。 \n\n\nNot Support Grab Mode yet!", "ERROR");
                            return false;
                        }
                    }

                    if (mode == DataNames.INV || mode == DataNames.SCA || mode == DataNames.SCA_CAL || mode == DataNames.SCA2 || mode == DataNames.SCA2_CAL || mode == DataNames.REF_CAL_F)
                    {
                        MessageBox.Show("不支持当前设置。 \n\n\nNot Support Grab Mode yet!", "ERROR");
                        return false;
                    }
                }
            }
            else
            {
                MessageBox.Show("读取参数 <DeviceScanType> 失败！！\n\n\nGet value of parameter <DeviceScanType> failed!!", "ERROR");
                return false;
            }

            return true;
        }
        private void toolStripComboBoxMode_Click(object sender, EventArgs e)
        {
            var status = R3(m_ip).getStatus();
            if (status == CAM_STATUS.CAM_IS_STARTED)
                Stop();
            if (status == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                toolStripComboBoxMode.SelectedIndex = (int)m_GrabMode; // revert to last avalible value
            }


            // check if Grab Mode match parameter setting.
            // Grab Mode is under control by parameter setting.
            var lstGrabMode = m_GrabMode;

            m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;
            if (checkGrabMode())
            {
                //var selectedId = toolStripComboBoxMode.SelectedIndex;
                //m_GrabMode = (DataNames)selectedId;
                m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;


                if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STOPPED) // does not support connected on purpose! 
                    Start();
            }
            else
            {
                toolStripComboBoxMode.SelectedIndex = (int)lstGrabMode; // revert to last avalible value
            }

        }


        private void btnImportCameraConfigration_Click(object sender, EventArgs e)
        {
            _on_btn_Click(0);
        }

        private void btnExportCameraConfigration_Click(object sender, EventArgs e)
        {
            _on_btn_Click(1);
        }

        private void btnImportCalibrationFile_Click(object sender, EventArgs e)
        {
            _on_btn_Click(2);
        }

        private void frmMain_FormClosed(object sender, FormClosedEventArgs e)
        {
            _Stop();
            _Disconnection();
        }

        private void treeView1_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            var str = e.Node.Text;
            if (str == "Ranger3") return;

            m_ip = str;

            // recover tree view icon
            for (int i = 0; i < treeView1.Nodes[0].Nodes.Count; ++i)
                treeView1.Nodes[0].Nodes[i].ForeColor = treeView1.Nodes[0].Nodes[i].Text == m_ip ? Color.Red : Color.Black;

            // change status to stop or disconnect
            var sta = R3(m_ip).getStatus();
            if (sta == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                var status = _Connection();
                if (status != CAM_STATUS.All_OK) { MessageBox.Show(status.ToString()); return; }
            }
            if (sta == CAM_STATUS.CAM_IS_STARTED)
            {
                var status = _Stop();
                if (status != CAM_STATUS.All_OK) { MessageBox.Show(status.ToString()); return; }
            }
            btnCameraStatus(R3(m_ip).getStatus());

            // expand propertyGrids 
            updateParamPropertyGrid();
        }

        private void btnCameraStatus(CAM_STATUS sta)
        {
            toolStripButtonConnection.ForeColor = Color.Black;
            toolStripButtonDisconnection.ForeColor = Color.Black;
            toolStripButtonStart.ForeColor = Color.Black;
            toolStripButtonStop.ForeColor = Color.Black;

            if (sta == CAM_STATUS.CAM_IS_CONNECTED)
            {
                toolStripButtonConnection.ForeColor     = Color.Red;
            }
            else if (sta == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                toolStripButtonDisconnection.ForeColor = Color.Red;
            }
            else if (sta == CAM_STATUS.CAM_IS_STARTED)
            {
                toolStripButtonStart.ForeColor = Color.Red;
            }
            else if (sta == CAM_STATUS.CAM_IS_STOPPED)
            {
                toolStripButtonStop.ForeColor = Color.Red;
            }
        }

        // id == 0 ImportConfigrationFile(); id == 1 ExportConfigrationFile(); id == 2 ImportCalibrationFile();
        private CAM_STATUS _on_btn_Click(int id)
        {
            var status = _Stop();
            if (CAM_STATUS.All_OK != status) { MessageBox.Show("Stop Camera failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }

            switch (id)
            {
                case 0:
                    status = ImportConfigrationFile();
                    if (CAM_STATUS.All_OK != status) { MessageBox.Show("Import Configration File failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }
                    break;
                case 1:
                    status = ExportConfigrationFile();
                    if (CAM_STATUS.All_OK != status) { MessageBox.Show("Export Configration File failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }
                    break;
                case 2:
                    status = ImportCalibrationFile();
                    if (CAM_STATUS.All_OK != status) { MessageBox.Show("Import Calibration File failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }
                    break;
            }
            MessageBox.Show("Processing OK !!");
            status = _Start();
            if (CAM_STATUS.All_OK != status) { MessageBox.Show("Start Camera failed!! ErrCode: " + status.ToString(), "ERROR"); return status; }

            return status;
        }

        private void unitTestToolStripMenuItem_Click(object sender, EventArgs e)
        {
#if UnitTest
            _UnitTest();
#endif
        }

        private void wFlog(String str)
        {
            lock (m_logLocker)
            {
                textBox1.Invoke(new EventHandler(delegate
                {
                    textBox1.Text = textBox1.Text + str + "\r\n\r\n";
                    textBox1.Select(textBox1.TextLength, 0);
                    textBox1.ScrollToCaret();
                }));
            }
            
        }

        private void btnSaveBuffer_Click(object sender, EventArgs e)
        {
            if (!SaveImage())
                MessageBox.Show("Save as ICON failed! 保存失败！");
            else
                MessageBox.Show("Save as ICON ok! 保存成功！");
        }

        private void propertyGrid1_Click(object sender, EventArgs e)
        {

        }

        private void 打开APIHelpToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start(@"Ranger3_CSharp_API_help.chm");
        }

        private void 使用指南ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Bitmap src = new Bitmap(@"help.PNG");
            IntPtr pp = new IntPtr();

            Form f = new Form();
            f.Text = "使用指南 | User Guide";
            f.BackgroundImage = src.GetThumbnailImage(src.Width, src.Height, null, pp);
            f.Size = new Size(src.Width, src.Height + 30);
            f.MaximumSize = new Size(src.Width, src.Height+30);
            f.Show();
        }

        private void toolStripButtonSetParam_Click(object sender, EventArgs e)
        {
            // camera status check
            var sta = R3(m_ip).getStatus();
            if (sta == CAM_STATUS.CAM_IS_STARTED)
                Stop();
            if (sta == CAM_STATUS.CAM_IS_DISCONNECTED)
                Connection();

            sta = R3(m_ip).getStatus();
            if (!(sta == CAM_STATUS.CAM_IS_STOPPED || sta == CAM_STATUS.CAM_IS_CONNECTED))
            {
                MessageBox.Show("相机状态不合理! ErrorCode: " + sta.ToString() 
                    + "Camera status invalid, ErrorCode: " + sta.ToString(), "ERROR");
                return;
            }

            // set para
            foreach (var prop in m_PropsParam)
            {
                if (prop.Update)
                {
                    var name = prop.Name;
                    String value = "";
                    if (prop.Options.Length > 0) // has option
                    {
                        int valueId = Convert.ToInt32(prop.Value.ToString());
                        if (valueId >= prop.Options.Length)
                        {
                            MessageBox.Show(CAM_STATUS.ERROR_PARAMETER_VALUE_INVALID.ToString(), "ERROR");
                            return;
                        }    
                        value = prop.Options[valueId];
                    }
                    else // numerical parameter
                        value = prop.Value.ToString();

                    var status = R3(m_ip).setParameterValue(name, value);
                    if (status != CAM_STATUS.All_OK)
                    {
                        MessageBox.Show("尝试设置 <" + name + "> = [" + value + "] 失败！ ErrorCode: " + status.ToString() 
                            + "Try to set <" + name + "> as [" + value + "] Failed! ErrorCode: " + status.ToString(), "ERROR");
                        return;
                    }
                }

            }

            if(checkGrabMode())
                MessageBox.Show("设置成功！\n\nSetting Done!", "OK");
        }

        private void btnLoadBuffer_Click(object sender, EventArgs e)
        {
            CSImgT img = LoadImage();
            if (!img.isEmpty())
            {
                var dnList = img.getAvalibleDataNames();
                foreach(var dn in dnList)
                {
                    Form f = new Form();
                    Bitmap src = BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(dn), img.get_rows(dn));

                    if (dn == DataNames.SEN)
                    {
                        f.Text = "激光线图像 | Sensor Image";
                        src = BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(dn), img.get_rows(dn));
                    }
                    else if(dn == DataNames.RAN)
                    {
                        f.Text = "高度图像 | Range Image";
                        src = BuiltGrayBitmap(img.getDataWord(dn), img.get_cols(dn), img.get_rows(dn));
                    }
                    else if (dn == DataNames.RAN_CAL)
                    {
                        f.Text = "标定后的高度图像 | Calibrated Range Image";
                        src = BuiltGrayBitmap(img.getDataFloat(dn), img.get_cols(dn), img.get_rows(dn));
                    }
                    else if (dn == DataNames.REF)
                    {
                        f.Text = "激光强度图像 | Reflectance Image";
                        src = BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(dn), img.get_rows(dn));
                    }
                    else if (dn == DataNames.REF_CAL)
                    {
                        f.Text = "标定后的激光强度图像 | Calibrated Reflectance Image";
                        src = BuiltGrayBitmap(img.getDataByte(dn), img.get_cols(dn), img.get_rows(dn));
                    }

                    IntPtr pp = new IntPtr();
                    Size imgSize = new Size(src.Width/3, src.Height/3);
                    f.BackgroundImage = src.GetThumbnailImage(imgSize.Width, imgSize.Height, null, pp);
                    f.Size = new Size(imgSize.Width, imgSize.Height + 30);
                    f.MaximumSize = new Size(imgSize.Width, imgSize.Height + 30);
                    f.Show();
                }
            }
        }

        private void 常见问题ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Bitmap src = new Bitmap(@"help2.PNG");
            IntPtr pp = new IntPtr();

            Form f = new Form();
            f.Text = "常见问题 | Q & A";
            f.BackgroundImage = src.GetThumbnailImage(src.Width, src.Height, null, pp);
            f.Size = new Size(src.Width, src.Height + 30);
            f.MaximumSize = new Size(src.Width, src.Height + 30);
            f.Show();
        }

        private void 版本VersionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var apiVersion = new SICK3D.VER();
            var vers = apiVersion.__Version__();
            MessageBox.Show(vers, "INFO");
        }


    }


}
