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

namespace Iris_2D_Game_Maker {
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window {
        public MainWindow() {
             InitializeComponent();
            _RegistEvent();
        }

        private void _RegistEvent() {
            btn_Close.Click += Btn_Close_Click;
            btn_Minimize.Click += Btn_Minimize_Click;
            lb_Title.MouseLeftButtonDown += Txt_Title_MouseLeftButtonDown;
        }

        private void Txt_Title_MouseLeftButtonDown(object sender, MouseButtonEventArgs e) {
            this.DragMove();
        }

        private void Btn_Minimize_Click(object sender, RoutedEventArgs e) {
            this.WindowState = WindowState.Minimized;
        }

        private void Btn_Close_Click(object sender, RoutedEventArgs e) {
            Application.Current.Shutdown();
        }
    }
}
