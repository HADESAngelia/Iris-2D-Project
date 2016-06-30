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
    /// IrisSmallIconButton.xaml 的交互逻辑
    /// </summary>
    public partial class IrisSmallIconButton : UserControl {

        public static readonly RoutedEvent ClickEvent =
            EventManager.RegisterRoutedEvent("Click", RoutingStrategy.Bubble, typeof(EventHandler<RoutedEventArgs>), typeof(IrisSmallIconButton));

        public event RoutedEventHandler Click
        {
            add { this.AddHandler(ClickEvent, value); }
            remove { this.RemoveHandler(ClickEvent, value); }
        }

        public String IconSource
        {
            get { return (String)GetValue(IconSourceProperty); }
            set { SetValue(IconSourceProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Source.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IconSourceProperty =
            DependencyProperty.Register("IconSource", typeof(String), typeof(IrisSmallIconButton), new PropertyMetadata("image/icon/appbar.minus.png"));

        public Color GradualColor
        {
            get { return (Color)GetValue(GruadualColorProperty); }
            set { SetValue(GruadualColorProperty, value); }
        }

        // Using a DependencyProperty as the backing store for GradualColor.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty GruadualColorProperty =
            DependencyProperty.Register("GradualColor", typeof(Color), typeof(IrisSmallIconButton), new PropertyMetadata(Colors.Blue));

        public IrisSmallIconButton() {
            InitializeComponent();

            btn_Base.Click += Btn_Base_Click;
        }

        private void Btn_Base_Click(object sender, RoutedEventArgs e) {
            this.RaiseEvent(new RoutedEventArgs(ClickEvent));
        }
    }
}
