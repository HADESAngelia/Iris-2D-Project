using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Iris_2D_Game_Maker {
    public class DepColor : DependencyObject {

        public Color FromColor
        {
            get { return (Color)GetValue(FromColorProperty); }
            set { SetValue(FromColorProperty, value); }
        }

        // Using a DependencyProperty as the backing store for FromColorProperty.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty FromColorProperty =
            DependencyProperty.Register("FromColor", typeof(Color), typeof(DepColor), new PropertyMetadata(Colors.Transparent));

        public Color ToColor
        {
            get { return (Color)GetValue(ToColorProperty); }
            set { SetValue(ToColorProperty, value); }
        }

        // Using a DependencyProperty as the backing store for ToColorProperty.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ToColorProperty =
            DependencyProperty.Register("ToColor", typeof(Color), typeof(DepColor), new PropertyMetadata(Colors.LightBlue));
    }
}
