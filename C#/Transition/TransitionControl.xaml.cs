using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfLearn
{
    /// <summary>
    /// TransitionControl.xaml 的交互逻辑
    /// </summary>
    public partial class TransitionControl : UserControl
    {
        public MainWindow ParentWindow { get; set; }
        public TransitionControl CurrentScreen { get; set; }


        public TransitionControl(MainWindow parentWindow)
        {
            InitializeComponent();
            this.ParentWindow = parentWindow;
        }

        public void ChangeScreen(TransitionControl screen)
        {
            if (screen == null) {
                throw new ArgumentNullException("Unable to navigate to next screen. A null reference section occured");
            }
            this.CurrentScreen = screen;
            this.ParentWindow.ChangeContent(screen);
        }
    }
}
