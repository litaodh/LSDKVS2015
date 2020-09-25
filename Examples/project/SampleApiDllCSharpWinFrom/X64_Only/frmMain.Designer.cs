namespace SampleApiDllCSharpWinForm
{
    partial class frmMain
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripMenuItem();
            this.打开APIHelpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.使用指南ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unitTestToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButtonFindCamera = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonConnection = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonDisconnection = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonStart = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonStop = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonSetParam = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripComboBoxMode = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.btnImportCameraConfigration = new System.Windows.Forms.ToolStripButton();
            this.btnExportCameraConfigration = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton7 = new System.Windows.Forms.ToolStripSeparator();
            this.btnImportCalibrationFile = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.btnLoadBuffer = new System.Windows.Forms.ToolStripButton();
            this.btnSaveBuffer = new System.Windows.Forms.ToolStripButton();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.splitContainer3 = new System.Windows.Forms.SplitContainer();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.tabPage6 = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.版本VersionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.常见问题ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
            this.splitContainer3.Panel1.SuspendLayout();
            this.splitContainer3.Panel2.SuspendLayout();
            this.splitContainer3.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.statusStrip1.Location = new System.Drawing.Point(0, 884);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Padding = new System.Windows.Forms.Padding(1, 0, 10, 0);
            this.statusStrip1.Size = new System.Drawing.Size(1366, 22);
            this.statusStrip1.TabIndex = 0;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // menuStrip1
            // 
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.toolStripMenuItem2,
            this.toolStripMenuItem3,
            this.toolStripMenuItem4,
            this.unitTestToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(4, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(1366, 25);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(44, 21);
            this.toolStripMenuItem1.Text = "文件";
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(44, 21);
            this.toolStripMenuItem2.Text = "视图";
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(44, 21);
            this.toolStripMenuItem3.Text = "选项";
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.打开APIHelpToolStripMenuItem,
            this.使用指南ToolStripMenuItem,
            this.常见问题ToolStripMenuItem,
            this.版本VersionToolStripMenuItem});
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(44, 21);
            this.toolStripMenuItem4.Text = "帮助";
            // 
            // 打开APIHelpToolStripMenuItem
            // 
            this.打开APIHelpToolStripMenuItem.Name = "打开APIHelpToolStripMenuItem";
            this.打开APIHelpToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.打开APIHelpToolStripMenuItem.Text = "打开 API help";
            this.打开APIHelpToolStripMenuItem.Click += new System.EventHandler(this.打开APIHelpToolStripMenuItem_Click);
            // 
            // 使用指南ToolStripMenuItem
            // 
            this.使用指南ToolStripMenuItem.Name = "使用指南ToolStripMenuItem";
            this.使用指南ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.使用指南ToolStripMenuItem.Text = "使用指南";
            this.使用指南ToolStripMenuItem.Click += new System.EventHandler(this.使用指南ToolStripMenuItem_Click);
            // 
            // unitTestToolStripMenuItem
            // 
            this.unitTestToolStripMenuItem.Name = "unitTestToolStripMenuItem";
            this.unitTestToolStripMenuItem.Size = new System.Drawing.Size(67, 21);
            this.unitTestToolStripMenuItem.Text = "UnitTest";
            this.unitTestToolStripMenuItem.Click += new System.EventHandler(this.unitTestToolStripMenuItem_Click);
            // 
            // toolStrip1
            // 
            this.toolStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonFindCamera,
            this.toolStripSeparator4,
            this.toolStripButtonConnection,
            this.toolStripButtonDisconnection,
            this.toolStripSeparator1,
            this.toolStripButtonStart,
            this.toolStripButtonStop,
            this.toolStripButtonSetParam,
            this.toolStripSeparator2,
            this.toolStripLabel1,
            this.toolStripComboBoxMode,
            this.toolStripSeparator3,
            this.btnImportCameraConfigration,
            this.btnExportCameraConfigration,
            this.toolStripButton7,
            this.btnImportCalibrationFile,
            this.toolStripSeparator5,
            this.btnLoadBuffer,
            this.btnSaveBuffer});
            this.toolStrip1.Location = new System.Drawing.Point(0, 25);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(1366, 27);
            this.toolStrip1.TabIndex = 2;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripButtonFindCamera
            // 
            this.toolStripButtonFindCamera.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonFindCamera.Image")));
            this.toolStripButtonFindCamera.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonFindCamera.Name = "toolStripButtonFindCamera";
            this.toolStripButtonFindCamera.Size = new System.Drawing.Size(59, 24);
            this.toolStripButtonFindCamera.Text = "Scan";
            this.toolStripButtonFindCamera.Click += new System.EventHandler(this.toolStripButtonFindCamera_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 27);
            // 
            // toolStripButtonConnection
            // 
            this.toolStripButtonConnection.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonConnection.Image")));
            this.toolStripButtonConnection.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonConnection.Name = "toolStripButtonConnection";
            this.toolStripButtonConnection.Size = new System.Drawing.Size(79, 24);
            this.toolStripButtonConnection.Text = "Connect";
            this.toolStripButtonConnection.Click += new System.EventHandler(this.toolStripButtonConnection_Click);
            // 
            // toolStripButtonDisconnection
            // 
            this.toolStripButtonDisconnection.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonDisconnection.Image")));
            this.toolStripButtonDisconnection.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonDisconnection.Name = "toolStripButtonDisconnection";
            this.toolStripButtonDisconnection.Size = new System.Drawing.Size(88, 24);
            this.toolStripButtonDisconnection.Text = "Disconect";
            this.toolStripButtonDisconnection.Click += new System.EventHandler(this.toolStripButtonDisconnection_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 27);
            // 
            // toolStripButtonStart
            // 
            this.toolStripButtonStart.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonStart.Image")));
            this.toolStripButtonStart.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonStart.Name = "toolStripButtonStart";
            this.toolStripButtonStart.Size = new System.Drawing.Size(59, 24);
            this.toolStripButtonStart.Text = "Start";
            this.toolStripButtonStart.Click += new System.EventHandler(this.toolStripButtonStart_Click);
            // 
            // toolStripButtonStop
            // 
            this.toolStripButtonStop.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonStop.Image")));
            this.toolStripButtonStop.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonStop.Name = "toolStripButtonStop";
            this.toolStripButtonStop.Size = new System.Drawing.Size(59, 24);
            this.toolStripButtonStop.Text = "Stop";
            this.toolStripButtonStop.Click += new System.EventHandler(this.toolStripButtonStop_Click);
            // 
            // toolStripButtonSetParam
            // 
            this.toolStripButtonSetParam.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonSetParam.Image")));
            this.toolStripButtonSetParam.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSetParam.Name = "toolStripButtonSetParam";
            this.toolStripButtonSetParam.Size = new System.Drawing.Size(91, 24);
            this.toolStripButtonSetParam.Text = "Set Param";
            this.toolStripButtonSetParam.Click += new System.EventHandler(this.toolStripButtonSetParam_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 27);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(76, 24);
            this.toolStripLabel1.Text = "Grab Mode";
            // 
            // toolStripComboBoxMode
            // 
            this.toolStripComboBoxMode.Items.AddRange(new object[] {
            "Invalid",
            "Sensor",
            "Range",
            "Reflectance",
            "Scatter",
            "Range_Calibrated",
            "Reflectance_Calibrated",
            "Reflectance_Calib_old",
            "Scatter_Calibrated",
            "Extra_Range",
            "Extra_Reflectance",
            "Extra_Scatter",
            "Extra_Range_Calibrated",
            "Extra_Reflectance_Calibrated",
            "Extra_Scatter_Calibrated"});
            this.toolStripComboBoxMode.Name = "toolStripComboBoxMode";
            this.toolStripComboBoxMode.Size = new System.Drawing.Size(150, 27);
            this.toolStripComboBoxMode.SelectedIndexChanged += new System.EventHandler(this.toolStripComboBoxMode_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 27);
            // 
            // btnImportCameraConfigration
            // 
            this.btnImportCameraConfigration.Image = ((System.Drawing.Image)(resources.GetObject("btnImportCameraConfigration.Image")));
            this.btnImportCameraConfigration.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnImportCameraConfigration.Name = "btnImportCameraConfigration";
            this.btnImportCameraConfigration.Size = new System.Drawing.Size(106, 24);
            this.btnImportCameraConfigration.Text = "Import Parm";
            this.btnImportCameraConfigration.Click += new System.EventHandler(this.btnImportCameraConfigration_Click);
            // 
            // btnExportCameraConfigration
            // 
            this.btnExportCameraConfigration.Image = ((System.Drawing.Image)(resources.GetObject("btnExportCameraConfigration.Image")));
            this.btnExportCameraConfigration.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnExportCameraConfigration.Name = "btnExportCameraConfigration";
            this.btnExportCameraConfigration.Size = new System.Drawing.Size(111, 24);
            this.btnExportCameraConfigration.Text = "Export Param";
            this.btnExportCameraConfigration.Click += new System.EventHandler(this.btnExportCameraConfigration_Click);
            // 
            // toolStripButton7
            // 
            this.toolStripButton7.Name = "toolStripButton7";
            this.toolStripButton7.Size = new System.Drawing.Size(6, 27);
            // 
            // btnImportCalibrationFile
            // 
            this.btnImportCalibrationFile.Image = ((System.Drawing.Image)(resources.GetObject("btnImportCalibrationFile.Image")));
            this.btnImportCalibrationFile.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnImportCalibrationFile.Name = "btnImportCalibrationFile";
            this.btnImportCalibrationFile.Size = new System.Drawing.Size(139, 24);
            this.btnImportCalibrationFile.Text = "Import Calibration";
            this.btnImportCalibrationFile.Click += new System.EventHandler(this.btnImportCalibrationFile_Click);
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(6, 27);
            // 
            // btnLoadBuffer
            // 
            this.btnLoadBuffer.Image = ((System.Drawing.Image)(resources.GetObject("btnLoadBuffer.Image")));
            this.btnLoadBuffer.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnLoadBuffer.Name = "btnLoadBuffer";
            this.btnLoadBuffer.Size = new System.Drawing.Size(88, 24);
            this.btnLoadBuffer.Text = "Load Img";
            this.btnLoadBuffer.Click += new System.EventHandler(this.btnLoadBuffer_Click);
            // 
            // btnSaveBuffer
            // 
            this.btnSaveBuffer.Image = ((System.Drawing.Image)(resources.GetObject("btnSaveBuffer.Image")));
            this.btnSaveBuffer.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSaveBuffer.Name = "btnSaveBuffer";
            this.btnSaveBuffer.Size = new System.Drawing.Size(86, 24);
            this.btnSaveBuffer.Text = "Save Img";
            this.btnSaveBuffer.Click += new System.EventHandler(this.btnSaveBuffer_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 52);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer3);
            this.splitContainer1.Size = new System.Drawing.Size(1366, 832);
            this.splitContainer1.SplitterDistance = 434;
            this.splitContainer1.TabIndex = 5;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.treeView1);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.propertyGrid1);
            this.splitContainer2.Size = new System.Drawing.Size(434, 832);
            this.splitContainer2.SplitterDistance = 333;
            this.splitContainer2.TabIndex = 0;
            // 
            // treeView1
            // 
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Margin = new System.Windows.Forms.Padding(2);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(434, 333);
            this.treeView1.TabIndex = 2;
            this.treeView1.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView1_NodeMouseDoubleClick);
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.Size = new System.Drawing.Size(434, 495);
            this.propertyGrid1.TabIndex = 2;
            // 
            // splitContainer3
            // 
            this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer3.Location = new System.Drawing.Point(0, 0);
            this.splitContainer3.Name = "splitContainer3";
            this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer3.Panel1
            // 
            this.splitContainer3.Panel1.Controls.Add(this.tabControl1);
            // 
            // splitContainer3.Panel2
            // 
            this.splitContainer3.Panel2.Controls.Add(this.groupBox1);
            this.splitContainer3.Size = new System.Drawing.Size(928, 832);
            this.splitContainer3.SplitterDistance = 562;
            this.splitContainer3.TabIndex = 0;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Controls.Add(this.tabPage6);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(2);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(928, 562);
            this.tabControl1.TabIndex = 8;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.pictureBox1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Margin = new System.Windows.Forms.Padding(2);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(2);
            this.tabPage1.Size = new System.Drawing.Size(920, 536);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "tabPage1";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox1.Location = new System.Drawing.Point(2, 2);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(916, 532);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // tabPage2
            // 
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Margin = new System.Windows.Forms.Padding(2);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(2);
            this.tabPage2.Size = new System.Drawing.Size(920, 536);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "tabPage2";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // tabPage3
            // 
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Margin = new System.Windows.Forms.Padding(2);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(920, 536);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "tabPage3";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // tabPage4
            // 
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Margin = new System.Windows.Forms.Padding(2);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(920, 536);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "tabPage4";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // tabPage5
            // 
            this.tabPage5.Location = new System.Drawing.Point(4, 22);
            this.tabPage5.Margin = new System.Windows.Forms.Padding(2);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Size = new System.Drawing.Size(920, 536);
            this.tabPage5.TabIndex = 4;
            this.tabPage5.Text = "tabPage5";
            this.tabPage5.UseVisualStyleBackColor = true;
            // 
            // tabPage6
            // 
            this.tabPage6.Location = new System.Drawing.Point(4, 22);
            this.tabPage6.Margin = new System.Windows.Forms.Padding(2);
            this.tabPage6.Name = "tabPage6";
            this.tabPage6.Size = new System.Drawing.Size(920, 536);
            this.tabPage6.TabIndex = 5;
            this.tabPage6.Text = "tabPage6";
            this.tabPage6.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.groupBox1.Controls.Add(this.textBox1);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox1.Size = new System.Drawing.Size(928, 266);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Log";
            // 
            // textBox1
            // 
            this.textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox1.Location = new System.Drawing.Point(2, 16);
            this.textBox1.Margin = new System.Windows.Forms.Padding(2);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(924, 248);
            this.textBox1.TabIndex = 0;
            // 
            // 版本VersionToolStripMenuItem
            // 
            this.版本VersionToolStripMenuItem.Name = "版本VersionToolStripMenuItem";
            this.版本VersionToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.版本VersionToolStripMenuItem.Text = "版本/Version";
            this.版本VersionToolStripMenuItem.Click += new System.EventHandler(this.版本VersionToolStripMenuItem_Click);
            // 
            // 常见问题ToolStripMenuItem
            // 
            this.常见问题ToolStripMenuItem.Name = "常见问题ToolStripMenuItem";
            this.常见问题ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.常见问题ToolStripMenuItem.Text = "常见问题";
            this.常见问题ToolStripMenuItem.Click += new System.EventHandler(this.常见问题ToolStripMenuItem_Click);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1366, 906);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "frmMain";
            this.Text = "Ranger3APIDemo";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.frmMain_FormClosed);
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.splitContainer3.Panel1.ResumeLayout(false);
            this.splitContainer3.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
            this.splitContainer3.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolStripButtonConnection;
        private System.Windows.Forms.ToolStripButton toolStripButtonDisconnection;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton toolStripButtonStart;
        private System.Windows.Forms.ToolStripButton toolStripButtonStop;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBoxMode;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton btnImportCameraConfigration;
        private System.Windows.Forms.ToolStripButton btnExportCameraConfigration;
        private System.Windows.Forms.ToolStripSeparator toolStripButton7;
        private System.Windows.Forms.ToolStripButton btnSaveBuffer;
        private System.Windows.Forms.ToolStripButton toolStripButtonFindCamera;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem4;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripButton btnImportCalibrationFile;
        private System.Windows.Forms.ToolStripMenuItem unitTestToolStripMenuItem;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.SplitContainer splitContainer3;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.TabPage tabPage6;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ToolStripMenuItem 打开APIHelpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 使用指南ToolStripMenuItem;
        private System.Windows.Forms.ToolStripButton toolStripButtonSetParam;
        private System.Windows.Forms.ToolStripButton btnLoadBuffer;
        private System.Windows.Forms.ToolStripMenuItem 常见问题ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 版本VersionToolStripMenuItem;
    }
}

