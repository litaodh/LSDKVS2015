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
using System.Threading;


using SICK3D;

namespace SampleApiDllCSharpWinForm
{
    public partial class frmMain : Form
    {
        private Ranger3 R3(String ip)
        {
            if (m_list_r3.ContainsKey(ip))
                return m_list_r3[ip];

            return new Ranger3("", true); // Invalid Ranger3 object. Please note, do not return null
        }

        private void FindCamera()
        {
            CAM_STATUS scanRes = CAM_STATUS.DEFAULT;
            if (m_r3s.isCtiFound())
            {
                scanRes = m_r3s.scanDevice();
            }

            if (scanRes == CAM_STATUS.All_OK)
            {
                TreeNode nodeRoot = new TreeNode("Ranger3");
                treeView1.Nodes.Add(nodeRoot);
                String[] IPs = m_r3s.getConDevListIP();
                for (int i = 0; i < IPs.Length; i++)
                {
                    TreeNode nodeChild = new TreeNode(IPs[i]);
                    nodeRoot.Nodes.Add(nodeChild);

                    m_list_r3.Add(IPs[i], new Ranger3(IPs[i], true));
                }

                m_ip = (IPs.Length > 0) ? IPs[0] : "" ;
            }

        }

        private void Connection()
        {
            var status = _Connection();
            if (status != CAM_STATUS.All_OK)
                MessageBox.Show(status.ToString());

            if (false == R3(m_ip).updateParamtersInfo())
            {
                MessageBox.Show("更新相机参数错误！！\n\n\nUpdate paramters failed!!");
                Disconnection();
            }
            // expand propertyGrids 
            updateParamPropertyGrid();
        }

        private void Disconnection()
        {
            var status = _Disconnection(); if (status != CAM_STATUS.All_OK) MessageBox.Show(status.ToString());
        }

        private void Start()
        {
            var status = _Start(); if (status != CAM_STATUS.All_OK) MessageBox.Show(status.ToString());
        }

        private void Stop()
        {
            var status = _Stop(); if (status != CAM_STATUS.All_OK) MessageBox.Show(status.ToString());
        }

        private CAM_STATUS _Connection()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            status = R3(m_ip).connectCamera(true);
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);

            // some gui setup
            var scanType = R3(m_ip).getParameterValue("DeviceScanType");
            toolStripComboBoxMode.SelectedIndex = scanType == "Areascan" ? (int)DataNames.SEN : (int)DataNames.RAN;
            //m_GrabMode = (DataNames)toolStripComboBoxMode.SelectedIndex;

            return status;
        }

        private CAM_STATUS _Disconnection()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_STARTED)
            {
                MessageBox.Show("Please Stop Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_STARTED;
            }
            status = R3(m_ip).disconnectCamera();
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);
            return status;
       }

        private CAM_STATUS _Start()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;
            if (R3(m_ip).getStatus() == CAM_STATUS.CAM_IS_DISCONNECTED)
            {
                MessageBox.Show("Please Connect Camera first!!");
                return CAM_STATUS.ERROR_CAM_IS_DISCONNECTED;
            }

            status = R3(m_ip).startCamera();
            myTimer.Enabled = status == CAM_STATUS.All_OK;
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);
            return status;
        }

        private CAM_STATUS _Stop()
        {
            var status = _checkR3(); if (status != CAM_STATUS.All_OK) return status;

            m_reqStop = 0; // send requirement
            while (m_reqStop == 0 && myTimer.Enabled) { Thread.Sleep(5); };
            m_reqStop = 2;

            status = R3(m_ip).stopCamera();
            myTimer.Enabled = !(status == CAM_STATUS.All_OK);
            btnCameraStatus(R3(m_ip).getStatus());
            Thread.Sleep(5);
            return status;
        }

        private CAM_STATUS ImportConfigrationFile()
        {
            string strConfigPath = string.Empty;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Please select a configuration file / 请选择相机配置文件";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "csv|*.csv";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return CAM_STATUS.ERROR_CSV_PATH;

            return R3(m_ip).setParametPath(strConfigPath);
        }

        private CAM_STATUS ExportConfigrationFile()
        {
            string strConfigPath = string.Empty;
            SaveFileDialog ofd = new SaveFileDialog();
            ofd.Title = "Please select a path to save configuration file / 请选择要保存相机配置文件的路径";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "csv|*.csv";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return CAM_STATUS.ERROR_CSV_PATH;

            return R3(m_ip).saveParameterToCSV(strConfigPath);
        }

        private CAM_STATUS ImportCalibrationFile()
        {
            string strConfigPath = string.Empty;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Please select a calibration file / 请选择标定文件";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "xml|*.xml|json|*.json";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return CAM_STATUS.ERROR_XML_PATH;

            return R3(m_ip).setCalibraPath(strConfigPath);
        }

        private bool SaveImage()
        {
            _Stop();

            string strConfigPath = string.Empty;
            SaveFileDialog ofd = new SaveFileDialog();
            ofd.Title = "Please select a path to save image / 请选择保存路径";
            ofd.InitialDirectory = Application.StartupPath;
            //ofd.Filter = "*.*";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return false;

            return m_lastImg.SaveToIconFile(strConfigPath, DataNames.INV);
        }

        private CSImgT LoadImage()
        {
            // This is just a demo
            MessageBox.Show("这里仅以代码形式演示如何加载 ICON(dat|xml) 类型的图像数据。");
            CSImgT img = new CSImgT();

            string strConfigPath = string.Empty;
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "Please select the path / 请选择加载路径";
            ofd.InitialDirectory = Application.StartupPath;
            ofd.Filter = "xml|*.xml";
            if (ofd.ShowDialog() == DialogResult.OK)
                strConfigPath = ofd.FileName;
            else
                return img;

            if (false == img.loadFromIconFile(strConfigPath))
                MessageBox.Show("加载失败！！ \n\n\nLoad Image failed!!");

            return img;
        }

        private DataNames GetGrabMode()
        {
            //return (DataNames)toolStripComboBoxMode.SelectedIndex;
            return m_GrabMode;
        }

        private CAM_STATUS _checkR3()
        {
            if(!m_r3s.isAvaliable())
                return CAM_STATUS.ERROR_NULL_DEV_HANDLE;

            if (m_ip == "" || R3(m_ip) == null)
            {
                MessageBox.Show("Please select a camera first!!");
                return CAM_STATUS.ERROR_NULL_PTR_R3;
            };

            if (R3(m_ip).isReady() != CAM_STATUS.All_OK)
            {
                MessageBox.Show("Camera is not ready!!");
                return CAM_STATUS.ERROR_NULL_PTR_R3;
            }
            return CAM_STATUS.All_OK;
        }

    }


}
