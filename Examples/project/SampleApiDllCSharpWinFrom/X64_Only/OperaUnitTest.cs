using System;
using System.Linq;
using System.Windows.Forms;
using System.Threading;
using SICK3D;


namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {
        public delegate void Action();

        bool isShowMessageBox = true;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////

        private void _UnitTest()
        {
            // start a thread
            var ok = MessageBox.Show("开始硬件单元测试，请确保相机已经连接。请将参数和视图的设置保持一致！\n\nUnit Test Start! Please make sure the camera is connected!", "注意 Note", 
                MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk);

            if (ok == DialogResult.Cancel)
                return;

            //System.Threading.ThreadStart threadStart = new ThreadStart(_UnitTestThreadRun);　　
            Thread thread = new Thread(()=>
            {
                Action a = new Action(_UnitTestThreadRun);
                BeginInvoke(a);
            });
            thread.Start();

        }

        private void _UnitTestThreadRun()
        {
            // UI status check
            {
                var sta = R3(m_ip).getStatus();
                if (sta == CAM_STATUS.CAM_IS_STARTED)
                    Stop();
                if (sta == CAM_STATUS.CAM_IS_DISCONNECTED)
                    Connection();

                sta = R3(m_ip).getStatus();
                if (sta == CAM_STATUS.CAM_IS_STOPPED || sta == CAM_STATUS.CAM_IS_CONNECTED)
                {
                    var scanType = R3(m_ip).getParameterValue("DeviceScanType");
                    var mode = GetGrabMode();
                    if ((mode == DataNames.SEN && scanType!= "Areascan") || (mode != DataNames.SEN && scanType != "Linescan3D"))
                    {
                        MessageBox.Show("检测到当前参数 <DeviceScanType> 设置为 [" + scanType + "]，与 Grab Mode 设置不匹配！\n" 
                            + "参数 [Areascan] 匹配 Grab Mode 为 Sensor;\n"
                            + "参数 [Linescan3D] 匹配 Grab Mode 为除了 Sensor 和 Invalid 之外的其他模式。\n\n\n"
                            + "Current <DeviceScanType> = [" + scanType + "], mismatch with Grab Mode.\n"
                            + "If [Areascan], the Grab Mode should be Sensor;\n"
                            + "If [Linescan3D], the Grab Mode should not be Sensor and Invalid.\n\n\n");
                        return;
                    }
                }
                else
                {
                    MessageBox.Show("相机状态异常，请检查后继续！ Camera status invalid, pls check and try again!");
                    return;
                }
            }

            // test 1, 4 status check
            bool res = true;
            int id = 0, total = 50;

            _TestFourStatus1(ref res, ref id, total);
            {
                var ok = MessageBox.Show("继续？ \n\nContinue?", "注意 Note", MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk);
                if (ok == DialogResult.Cancel) return;
            }

            _TestFourStatus2(ref res, ref id, total);
            {
                var ok = MessageBox.Show("继续？ \n\nContinue?", "注意 Note", MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk);
                if (ok == DialogResult.Cancel) return;
            }

            _TestParamFile(ref res, ref id, total);
            {
                var ok = MessageBox.Show("继续？ \n\nContinue?", "注意 Note", MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk);
                if (ok == DialogResult.Cancel) return;
            } 

            _TestParamSetting(ref res, ref id, total);
            {
                var ok = MessageBox.Show("继续？ \n\nContinue?", "注意 Note", MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk);
                if (ok == DialogResult.Cancel) return;
            }

            _TestSaveLoadImage(ref res, ref id, total);
            {
                var ok = MessageBox.Show("继续？ \n\nContinue?", "注意 Note", MessageBoxButtons.OKCancel, MessageBoxIcon.Asterisk);
                if (ok == DialogResult.Cancel) return;
            }

            _pass_or_fail(res, id++, total, "Final ...");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////

        private void _TestFourStatus1(ref bool res, ref int id, int total)
        {
            /// --- scan and find
            {
                res = res & _pass_or_fail(m_ip != "", id++, total, "IP");
                res = res & _pass_or_fail(m_list_r3.ContainsKey(m_ip), id++, total, "Divice list");
            }

            /// --- basic process: in order
            {
                Connection();
                Start(); Thread.Sleep(1000);
                Stop();
                Disconnection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_DISCONNECTED, id++, total, "in order");
            }
            Stop();
            Disconnection();
            /// --- basic process: 
            {
                Connection();
                Connection();
                Connection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_CONNECTED, id++, total, "connect repeatly");
            }
            Stop();
            Disconnection();
            /// --- basic process: start repeatly 4
            {
                Connection();
                Start(); Thread.Sleep(1000);
                Start(); Thread.Sleep(1000);
                Start(); Thread.Sleep(1000);
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STARTED, id++, total, "start repeatly 4");
            }
            Stop();
            Disconnection();
            /// --- basic process: stop repeatly
            {
                Connection();
                Start(); Thread.Sleep(1000);
                Stop();
                Stop();
                Stop();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STOPPED, id++, total, "stop repeatly");
            }
            Stop();
            Disconnection();
            /// --- basic process: disconnect repeatly
            {
                Connection();
                Start(); Thread.Sleep(1000);
                Stop();
                Disconnection();
                Disconnection();
                Disconnection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_DISCONNECTED, id++, total, "disconnect repeatly");
            }
            Stop();
            Disconnection();
            /// --- basic process: connect -> start, = start
            {
                Connection();
                Start(); Thread.Sleep(1000);
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STARTED, id++, total, "connect -> start, = start");
            }
            Stop();
            Disconnection();
            /// --- basic process: connect -> stop, = connect
            {
                Connection();
                Stop();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_CONNECTED, id++, total, "connect -> stop, = connect");
            }
            Stop();
            Disconnection();
            /// --- basic process: connect -> disconnect, = disconnect
            {
                Connection();
                Disconnection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_DISCONNECTED, id++, total, "connect -> disconnect, = disconnect");
            }
            /// --- basic process: start -> connect, = start 10
            {
                Connection();
                Start(); Thread.Sleep(1000);
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STARTED, id++, total, "start -> connect, = start");
            }
            Stop();
            Disconnection();
            /// --- basic process: start -> stop, = stop
            {
                Connection();
                Start(); Thread.Sleep(1000);
                Stop();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STOPPED, id++, total, "start -> stop, = stop");
            }
        }

        private void _TestFourStatus2(ref bool res, ref int id, int total)
        {
            Stop();
            Disconnection();
            /// --- basic process: start -> disconnect, = start
            {
                Connection();
                Start(); Thread.Sleep(1000);
                Disconnection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STARTED, id++, total, "start -> disconnect, = start");
            }
            Stop();
            Disconnection();
            /// --- basic process: stop -> start, = start
            {
                Connection();
                Start(); Thread.Sleep(1000);

                Stop();
                Start(); Thread.Sleep(1000);
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STARTED, id++, total, "stop -> start, = start");
            }
            Stop();
            Disconnection();
            /// --- basic process: stop -> connect, = stop
            {
                Connection();
                Start(); Thread.Sleep(1000);

                Stop();
                Connection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STOPPED, id++, total, "stop -> connect, = stop");
            }
            Stop();
            Disconnection();
            /// --- basic process: stop -> disconnect, = disconnect 15
            {
                Connection();
                Start(); Thread.Sleep(1000);

                Stop();
                Disconnection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_DISCONNECTED, id++, total, "stop -> disconnect, = disconnect");
            }
            Stop();
            Disconnection();
            /// --- basic process: disconnect -> start, = disconnect
            {
                Disconnection();
                Start(); Thread.Sleep(1000);
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_DISCONNECTED, id++, total, "disconnect -> start, = disconnect");
            }
            Stop();
            Disconnection();
            /// --- basic process: disconnect -> stop, = disconnect
            {
                Disconnection();
                Stop();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_DISCONNECTED, id++, total, "disconnect -> stop, = disconnect");
            }
            Stop();
            Disconnection();
            /// --- basic process: disconnect -> connect, = connect
            {
                Disconnection();
                Connection();
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_CONNECTED, id++, total, "disconnect -> connect, = connect");
            }
            Stop();
            Disconnection();
        }

        private void _TestParamFile(ref bool res, ref int id, int total)
        {
            /// --- export Configration file, after connect 19
            {
                Connection();
                var sta = _on_btn_Click(1);
                res = res & _pass_or_fail(CAM_STATUS.All_OK == sta, id++, total, "Export Configration File, after connect");
                res = res & _pass_or_fail(CAM_STATUS.CAM_IS_STARTED, id++, total);
            }
            Thread.Sleep(1000);
            Stop();


            Random rd = new Random();
            int targetExpoTimeRegion1 = rd.Next(10, 1000);
            int originExpoTimeRegion1 = 0;

            /// --- set new exposure time of region 1;
            {
                originExpoTimeRegion1 = Convert.ToInt32(R3(m_ip).getParameterValue("ExposureTime_RegionSelector_Region1"));

                var sta = R3(m_ip).setParameterValue("ExposureTime_RegionSelector_Region1", (targetExpoTimeRegion1).ToString());
                res = res & _pass_or_fail(CAM_STATUS.All_OK == sta, id++, total,
                    "Set new ExposureTime_RegionSelector_Region1, ErrCode: " + sta.ToString());

                int currentExpoTimeRegion1 = Convert.ToInt32(R3(m_ip).getParameterValue("ExposureTime_RegionSelector_Region1"));

                res = res & _pass_or_fail(currentExpoTimeRegion1 != originExpoTimeRegion1, id++, total,
                    "Set new ExposureTime_RegionSelector_Region1, ori=" + originExpoTimeRegion1.ToString());
                res = res & _pass_or_fail(currentExpoTimeRegion1 == targetExpoTimeRegion1, id++, total,
                    "Set new ExposureTime_RegionSelector_Region1, tar=" + targetExpoTimeRegion1.ToString() + "; cur=" + currentExpoTimeRegion1.ToString());
            }

            MessageBox.Show("加载 csv 文件时，请选择上步保存的 csv 文件。\n\n");

            /// --- import original Configration file
            {
                var sta = _on_btn_Click(0);
                res = res & _pass_or_fail(CAM_STATUS.All_OK == sta, id++, total, "Import original Configration file");
            }
            Thread.Sleep(1000);

            /// --- get original exposure time of region 1;
            Stop();
            {
                int currentExpoTimeRegion1 = Convert.ToInt32(R3(m_ip).getParameterValue("ExposureTime_RegionSelector_Region1"));
                res = res & _pass_or_fail(currentExpoTimeRegion1 == originExpoTimeRegion1, id++, total,
                    "Get original ExposureTime_RegionSelector_Region1, ori=" + originExpoTimeRegion1.ToString() + "; cur=" + currentExpoTimeRegion1.ToString());
            }

            /// --- import calibration file
            {
                var sta = _on_btn_Click(2);
                toolStripComboBoxMode.SelectedIndex = 5;

                res = res & _pass_or_fail(CAM_STATUS.All_OK == sta, id++, total, "Import Calibration file");
            }
        }

        private bool __checkParamValue(String para, String value1, String value2)
        {
            var cV = R3(m_ip).getParameterValue(para);

            {
                if (CAM_STATUS.All_OK != R3(m_ip).setParameterValue(para, value1))
                    return false;

                var ccV = R3(m_ip).getParameterValue(para);
                if (ccV != value1)   return false;
            }

            {
                if (CAM_STATUS.All_OK != R3(m_ip).setParameterValue(para, value2))
                    return false;

                var ccV = R3(m_ip).getParameterValue(para);
                if (ccV != value2) return false;
            }

            {
                if (CAM_STATUS.All_OK != R3(m_ip).setParameterValue(para, cV))
                    return false;

                var ccV = R3(m_ip).getParameterValue(para);
                if (ccV != cV) return false;
            }

            return true;
        }
        private void _TestParamSetting(ref bool res, ref int id, int total)
        {
            if (m_PropsParam.Count() == 0)
            {
                MessageBox.Show("请双击 IP，展开 参数列表后再试！ Please double click IP to expand parameters list first, try again later!");
                return;
            }

            Stop();
            Disconnection();
            Connection();

            if (CAM_STATUS.All_OK != R3(m_ip).saveParameterToCSV("tempFile_UnitTest_param.csv"))
            {
                MessageBox.Show("在测试参数设置之前，导出参数文件错误！这是不应该发生的问题！\n\n Fatal error! Happened when exporting parameter file before setting parameters test!");
                return;
            }

            // 
            res = res & _pass_or_fail(false, __checkParamValue("DeviceScanType", "Areascan", "Linescan3D"), id++, total, "DeviceScanType");
            res = res & _pass_or_fail(false, __checkParamValue("ExposureTime_RegionSelector_Region0", "400", "700"), id++, total, "ExposureTime_RegionSelector_Region0");
            res = res & _pass_or_fail(false, __checkParamValue("ExposureTime_RegionSelector_Region1", "400", "700"), id++, total, "ExposureTime_RegionSelector_Region1");
            res = res & _pass_or_fail(false, __checkParamValue("Width_RegionSelector_Region1", "2000", "2560"), id++, total, "Width_RegionSelector_Region1");
            res = res & _pass_or_fail(false, __checkParamValue("Height_RegionSelector_Region1", "200", "200"), id++, total, "Height_RegionSelector_Region1"); // only 200 can get OK???
            res = res & _pass_or_fail(false, __checkParamValue("Height_RegionSelector_Scan3dExtraction1", "400", "700"), id++, total, "Height_RegionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("OffsetX_RegionSelector_Region1", "0", "0"), id++, total, "OffsetX_RegionSelector_Region1");
            res = res & _pass_or_fail(false, __checkParamValue("OffsetY_RegionSelector_Region1", "0", "0"), id++, total, "OffsetY_RegionSelector_Region1");
            res = res & _pass_or_fail(false, __checkParamValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "0", "1"), id++, total, "ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("PixelFormat_ComponentSelector_Range_RegionSelector_Scan3dExtraction1", "Coord3D_C12p", "Mono12p"), id++, total, "PixelFormat_ComponentSelector_Range_RegionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("PixelFormat_ComponentSelector_Range_RegionSelector_Scan3dExtraction1", "Mono16", "Coord3D_C16"), id++, total, "PixelFormat_ComponentSelector_Range_RegionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("WamSize_Scan3dExtractionSelector_Scan3dExtraction1", "Small", "Large"), id++, total, "WamSize_Scan3dExtractionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("SearchMode3D_Scan3dExtractionSelector_Scan3dExtraction1", "GlobalMax", "FirstLocalMax"), id++, total, "SearchMode3D_Scan3dExtractionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("RangeAxis_Scan3dExtractionSelector_Scan3dExtraction1", "Standard", "Reverse"), id++, total, "RangeAxis_Scan3dExtractionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("DetectionThreshold_Scan3dExtractionSelector_Scan3dExtraction1", "50", "30"), id++, total, "DetectionThreshold_Scan3dExtractionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("ReflectanceFilter_Scan3dExtractionSelector_Scan3dExtraction1", "1", "0"), id++, total, "ReflectanceFilter_Scan3dExtractionSelector_Scan3dExtraction1");
            res = res & _pass_or_fail(false, __checkParamValue("AcquisitionMode", "SingleFrame", "Continuous"), id++, total, "AcquisitionMode");
            res = res & _pass_or_fail(false, __checkParamValue("AcquisitionFrameRate", "30", "21"), id++, total, "AcquisitionFrameRate");
            res = res & _pass_or_fail(false, __checkParamValue("AcquisitionLineRate", "1000", "1000"), id++, total, "AcquisitionLineRate");
            res = res & _pass_or_fail(false, __checkParamValue("TriggerMode_TriggerSelector_LineStart", "Off", "On"), id++, total, "TriggerMode_TriggerSelector_LineStart");
            res = res & _pass_or_fail(false, __checkParamValue("TriggerSource_TriggerSelector_LineStart", "FrameTriggerInput", "FrameTriggerInput"), id++, total, "TriggerSource_TriggerSelector_LineStart");
            res = res & _pass_or_fail(false, __checkParamValue("TriggerSource_TriggerSelector_ExposureStart", "LineStart", "LineStart"), id++, total, "TriggerSource_TriggerSelector_ExposureStart");
            res = res & _pass_or_fail(false, __checkParamValue("EncoderDivider", "4", "10"), id++, total, "EncoderDivider");
            res = res & _pass_or_fail(false, __checkParamValue("EncoderOutputMode", "DirectionUp", "PositionUp"), id++, total, "EncoderOutputMode");
            res = res & _pass_or_fail(false, __checkParamValue("EncoderOutputMode", "PositionDown", "DirectionDown"), id++, total, "EncoderOutputMode");
            res = res & _pass_or_fail(false, __checkParamValue("EncoderOutputMode", "Motion", "DirectionDown"), id++, total, "EncoderOutputMode");
            res = res & _pass_or_fail(false, __checkParamValue("ChunkModeActive", "0", "1"), id++, total, "ChunkModeActive");


            if (CAM_STATUS.All_OK != R3(m_ip).loadParameterFrCSV("tempFile_UnitTest_param.csv"))
            {
                MessageBox.Show("在测试参数设置之后，导入参数文件错误！这是不应该发生的问题！\n\n Fatal error! Happened when importing parameter file after setting parameters test!");
                return;
            }

            res = res & _pass_or_fail(true, id++, total, "Set and get parameters");
        }

        private void _TestSaveLoadImage(ref bool res, ref int id, int total)
        {
            MessageBox.Show("The image is here: D:\\support\\test");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////


        private bool _pass_or_fail(CAM_STATUS tar, int cid, int total, String str = "")
        {
            var cStatus = R3(m_ip).getStatus();
            bool res = cStatus == tar;
            wFlog(cid.ToString() + "/" + total.ToString() + (res ? " - OK! - ":" - NG! - ") + "CamStatus: " + cStatus.ToString()+ " - " +  str);
            if(isShowMessageBox)
                MessageBox.Show(cid.ToString() + "/" + total.ToString() + (res ? " - OK! - " : " - NG! - ") + "CamStatus: " + cStatus.ToString() + " - " + str);

            return res;
        }

        private bool _pass_or_fail(bool res, int cid, int total, String str="")
        {
            wFlog(cid.ToString() + "/" + total.ToString() + (res ? " - OK!" : " - NG!") + " - " + str);
            if (isShowMessageBox)
                MessageBox.Show(cid.ToString() + "/" + total.ToString() + (res ? " - OK!" : " - NG!") + " - " + str);

            return res;
        }

        private bool _pass_or_fail(bool show, bool res, int cid, int total, String str = "")
        {
            wFlog(cid.ToString() + "/" + total.ToString() + (res ? " - OK!" : " - NG!") + " - " + str);
            if (show)
                MessageBox.Show(cid.ToString() + "/" + total.ToString() + (res ? " - OK!" : " - NG!") + " - " + str);

            return res;
        }

    }


}
