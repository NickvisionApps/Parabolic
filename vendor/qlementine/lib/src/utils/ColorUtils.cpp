// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#include <oclero/qlementine/utils/ColorUtils.hpp>

#include <QRegularExpression>

#include <QDebug>

namespace oclero::qlementine {
constexpr auto HEX_BASE = 16;

double getContrastRatio(const QColor& c1, const QColor& c2) {
  Q_UNUSED(c1);
  Q_UNUSED(c2);
  // First, pre-compose c1 and c2 over plain white.

  /*
     * https://www.w3.org/TR/WCAG/#contrast-minimum
     * https://www.w3.org/TR/WCAG/#dfn-contrast-ratio
     *
     ratio = (L1 + 0.05) / (L2 + 0.05)

        L1 is the relative luminance of the lighter of the colors, and
        L2 is the relative luminance of the darker of the colors.

     with L = 0.2126 * R + 0.7152 * G + 0.0722 * B where R, G and B are defined as:

    if RsRGB <= 0.03928 then R = RsRGB/12.92 else R = ((RsRGB+0.055)/1.055) ^ 2.4
    if GsRGB <= 0.03928 then G = GsRGB/12.92 else G = ((GsRGB+0.055)/1.055) ^ 2.4
    if BsRGB <= 0.03928 then B = BsRGB/12.92 else B = ((BsRGB+0.055)/1.055) ^ 2.4

    and RsRGB, GsRGB, and BsRGB are defined as:

    RsRGB = R8bit/255
    GsRGB = G8bit/255
    BsRGB = B8bit/255

    Ratio should be at least 4.5:1
     */
  return 4.5;
}

QColor colorWithAlphaF(QColor const& color, qreal alpha) {
  alpha = std::min(1., std::max(0., alpha));
  auto result = QColor{ color };
  result.setAlphaF(alpha);
  return result;
}

QColor colorWithAlpha(QColor const& color, int alpha) {
  alpha = std::min(255, std::max(0, alpha));
  auto result = QColor{ color };
  result.setAlpha(alpha);
  return result;
}

QColor getColorSourceOver(const QColor& bg, const QColor& fg) {
  // Premultiply.
  const auto bgAlpha = bg.alphaF();
  const auto bgRed = bg.redF() * bgAlpha;
  const auto bgGreen = bg.greenF() * bgAlpha;
  const auto bgBlue = bg.blueF() * bgAlpha;

  const auto fgAlpha = fg.alphaF();
  const auto fgRed = fg.redF() * fgAlpha;
  const auto fgGreen = fg.greenF() * fgAlpha;
  const auto fgBlue = fg.blueF() * fgAlpha;
  const auto fgAlphaInv = 1. - fg.alphaF();

  const auto finalAlpha = bgAlpha + fgAlpha - bgAlpha * fgAlpha;
  const auto finalRed = fgRed + bgRed * fgAlphaInv;
  const auto finalGreen = fgGreen + bgGreen * fgAlphaInv;
  const auto finalBlue = fgBlue + bgBlue * fgAlphaInv;

  const auto finalRGBA = qRgba(int(finalRed * 255), int(finalGreen * 255), int(finalBlue * 255), int(finalAlpha * 255));
  auto finalColor = QColor::fromRgba(finalRGBA);

  return finalColor;
}

std::optional<QColor> tryGetColorFromVariantList(QVariantList const& variantList) {
  const auto variantListSize = variantList.size();
  if (variantListSize == 3 || variantListSize == 4) {
    int r{ 0 }, g{ 0 }, b{ 0 }, a{ 255 };

    const auto& variant_r = variantList.at(0);
    if (variant_r.isValid() && variant_r.canConvert<int>()) {
      r = variant_r.toInt();
    }
    const auto& variant_g = variantList.at(1);
    if (variant_g.isValid() && variant_g.canConvert<int>()) {
      g = variant_g.toInt();
    }
    const auto& variant_b = variantList.at(2);
    if (variant_b.isValid() && variant_b.canConvert<int>()) {
      b = variant_b.toInt();
    }
    if (variantListSize == 4) {
      const auto& variant_a = variantList.at(3);
      if (variant_a.isValid() && variant_a.canConvert<int>()) {
        a = variant_a.toInt();
      }
    }
    return QColor{ r, g, b, a };
  }

  return {};
}

std::optional<QColor> tryGetColorFromRGBAString(QString const& str) {
  // Try parse RGB ("rgb(RRR,GGG,BBB)").
  static const auto rgbRegExp =
    QRegularExpression(QStringLiteral("^ *rgb *\\( *(\\d{1,3}) *, *(\\d{1,3}) *, *(\\d{1,3}) *\\) *$"),
      QRegularExpression::CaseInsensitiveOption);
  const auto rgbMatch = rgbRegExp.match(str);
  if (rgbMatch.hasMatch()) {
    const auto r = rgbMatch.captured(1).toInt();
    const auto g = rgbMatch.captured(2).toInt();
    const auto b = rgbMatch.captured(3).toInt();
    return QColor{ r, g, b };
  }

  // Try parse RGBA ("rgba(RRR,GGG,BBB,AAA)").
  static const auto rgbaRegExp =
    QRegularExpression(QStringLiteral("^ *rgba *\\( *(\\d{1,3}) *, *(\\d{1,3}) *, *(\\d{1,3}) *, *(\\d{1,3})*\\) *$"),
      QRegularExpression::CaseInsensitiveOption);
  const auto rgbaMatch = rgbaRegExp.match(str);
  if (rgbaMatch.hasMatch()) {
    const auto r = rgbaMatch.captured(1).toInt();
    const auto g = rgbaMatch.captured(2).toInt();
    const auto b = rgbaMatch.captured(3).toInt();
    const auto a = rgbaMatch.captured(4).toInt();
    return QColor{ r, g, b, a };
  }

  return {};
}

std::optional<QColor> tryGetColorFromHexaString(QString const& str) {
  constexpr auto RGB_LENGTH = 3 * 2 + 1;
  constexpr auto RGBA_LENGTH = 4 * 2 + 1;

  const auto length = str.length();
  if (str.startsWith('#') && (length == RGB_LENGTH || length == RGBA_LENGTH)) {
    auto success{ false };

    const auto r_str = str.mid(1, 2);
    const auto g_str = str.mid(3, 2);
    const auto b_str = str.mid(5, 2);
    const auto a_str = str.mid(7, 2);

    const auto r = r_str.toInt(&success, HEX_BASE);
    if (success) {
      const auto g = g_str.toInt(&success, HEX_BASE);
      if (success) {
        const auto b = b_str.toInt(&success, HEX_BASE);
        if (success) {
          QColor result(r, g, b);

          const auto a = a_str.toInt(&success, HEX_BASE);
          if (success) {
            result.setAlpha(a);
          }

          return result;
        }
      }
    }
  }

  return {};
}

std::optional<QColor> tryGetColorFromVariant(QVariant const& variant) {
  const auto variantType = variant.typeId();

  // Channel list ([RRR, GGG, BBB, AAA]).
  if (variantType == QMetaType::Type::QVariantList) {
    const auto color = tryGetColorFromVariantList(variant.toList());
    if (color.has_value()) {
      return color.value();
    }
  }
  // Various ways to write color as a string.
  else if (variantType == QMetaType::Type::QString) {
    const auto variantString = variant.toString();

    // Check if color is written as hexadecimal.
    const auto hexaColor = tryGetColorFromHexaString(variantString);
    if (hexaColor.has_value()) {
      return hexaColor.value();
    }

    // Check if color is written as RGB(A).
    const auto rgbaColor = tryGetColorFromRGBAString(variantString);
    if (rgbaColor.has_value()) {
      return rgbaColor.value();
    }
  }

  return {};
}

QString toHexRGB(const QColor& color) {
  return QString("#%1%2%3")
    .arg(QString::number(color.red(), HEX_BASE), 2, '0')
    .arg(QString::number(color.green(), HEX_BASE), 2, '0')
    .arg(QString::number(color.blue(), HEX_BASE), 2, '0');
}

QString toHexRGBA(const QColor& color) {
  return QString("#%1%2%3%4")
    .arg(QString::number(color.red(), HEX_BASE), 2, '0')
    .arg(QString::number(color.green(), HEX_BASE), 2, '0')
    .arg(QString::number(color.blue(), HEX_BASE), 2, '0')
    .arg(QString::number(color.alpha(), HEX_BASE), 2, '0');
}
} // namespace oclero::qlementine
