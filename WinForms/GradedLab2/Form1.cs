using System.IO.Compression;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace GradedLab2
{
    public partial class Form1 : Form
    {

        private List<LocEntry> locEntries;

        public Form1()
        {
            InitializeComponent();
            this.tabControl1.SelectedIndex = 1;
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openFileDialog = new OpenFileDialog())
            {
                openFileDialog.InitialDirectory = Directory.GetCurrentDirectory();
                openFileDialog.Filter = "Loc Archive (*.zip)|*.zip";
                //openFileDialog.FilterIndex = 2;
                //openFileDialog.RestoreDirectory = true;

                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    //Get the path of specified file
                    var filePath = openFileDialog.FileName;

                  

                    ZipArchive locArchive = ZipFile.Open(filePath, ZipArchiveMode.Read);
                    this.locEntries = new();

                    locEntryBindingSource1.DataSource = this.locEntries;

                    foreach (ZipArchiveEntry entry in locArchive.Entries)
                    {
                        var x = JsonSerializer.Deserialize<LocEntry>(entry.Open());
                        this.locEntries.Add(x);
                    }
                }
               
            }
        }

        private void openFileDialog1_FileOk(object sender, System.ComponentModel.CancelEventArgs e)
        {

        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {

        }
    }
}