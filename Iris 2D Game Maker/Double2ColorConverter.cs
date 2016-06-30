using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Windows.Media;

namespace Iris_2D_Game_Maker {
    class Double2ColorConverter : IMultiValueConverter {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture) {            
            double rate = (double)values[0];
            Color fromColor = Colors.Transparent;
            Color toColor = (Color)values[1];
            byte targetR = (byte)(fromColor.R + rate * (toColor.R - fromColor.R));
            byte targetG = (byte)(fromColor.G + rate * (toColor.G - fromColor.G));
            byte targetB = (byte)(fromColor.B + rate * (toColor.B - fromColor.B));
            byte targetA = (byte)(fromColor.A + rate * (toColor.A - fromColor.A));

            Color targetColor = Color.FromArgb(targetA, targetR, targetG, targetB);

            return new SolidColorBrush(targetColor);
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture) {
            throw new NotImplementedException();
        }
    }
}
