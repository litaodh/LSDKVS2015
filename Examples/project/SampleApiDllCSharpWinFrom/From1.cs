using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
//

using CSSickCam;

namespace SampleApiDllCSharpWinFrom
{
    public partial class From1 : Form
    {
        #region
        CSR3S r3s = new CSR3S();
        #endregion
        public From1()
        {
            InitializeComponent();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            FindCamera();
        }

        private void FindCamera()
        {
            if (r3s != null)
            {
                bool scanRes = r3s.scanDevice();
                String[] IPs = r3s.getConDevListIP();
                String[] Nas = r3s.getConDevList();
            }
        }
    }
}
