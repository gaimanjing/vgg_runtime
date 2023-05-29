#pragma once
#include "Basic/VGGType.h"
#include "Basic/Attrs.h"
#include "Basic/Scene.hpp"

#include "include/core/SkBlendMode.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkImage.h"
#include "include/pathops/SkPathOps.h"

#include <vector>
#include <unordered_map>
#include <string>
using namespace VGG;

extern std::unordered_map<std::string, sk_sp<SkImage>> SkiaImageRepo;

#define SWITCH_MAP_ITEM_BEGIN(var)                                                                 \
  switch (var)                                                                                     \
  {

#define SWITCH_MAP_ITEM_DEF(from, to)                                                              \
  case from:                                                                                       \
    return to;

#define SWITCH_MAP_ITEM_DEF_NULL(from) case from:;

#define SWITCH_MAP_ITEM_END(fallback)                                                              \
  default:                                                                                         \
    return fallback;                                                                               \
    }

inline SkPaint::Join toSkPaintJoin(VGG::ELineJoin join)
{
  SWITCH_MAP_ITEM_BEGIN(join)
  SWITCH_MAP_ITEM_DEF(LJ_Miter, SkPaint::kMiter_Join)
  SWITCH_MAP_ITEM_DEF(LJ_Round, SkPaint::kRound_Join)
  SWITCH_MAP_ITEM_DEF(LJ_Bevel, SkPaint::kBevel_Join)
  SWITCH_MAP_ITEM_END(SkPaint::kMiter_Join)
}

inline SkBlendMode toSkBlendMode(EBlendMode mode)
{
  SWITCH_MAP_ITEM_BEGIN(mode)
  SWITCH_MAP_ITEM_DEF(BM_Normal, SkBlendMode::kSrcOver)
  SWITCH_MAP_ITEM_DEF(BM_Darken, SkBlendMode::kDarken)
  SWITCH_MAP_ITEM_DEF(BM_Multiply, SkBlendMode::kMultiply)
  SWITCH_MAP_ITEM_DEF(BM_Color_burn, SkBlendMode::kColorBurn)
  SWITCH_MAP_ITEM_DEF(BM_Lighten, SkBlendMode::kLighten)
  SWITCH_MAP_ITEM_DEF(BM_Screen, SkBlendMode::kScreen)
  SWITCH_MAP_ITEM_DEF(BM_Color_dodge, SkBlendMode::kColorDodge)
  SWITCH_MAP_ITEM_DEF(BM_Overlay, SkBlendMode::kOverlay)
  SWITCH_MAP_ITEM_DEF(BM_Soft_light, SkBlendMode::kSoftLight)
  SWITCH_MAP_ITEM_DEF(BM_Hard_light, SkBlendMode::kHardLight)
  SWITCH_MAP_ITEM_DEF(BM_Difference, SkBlendMode::kDifference)
  SWITCH_MAP_ITEM_DEF(BM_Exclusion, SkBlendMode::kExclusion)
  SWITCH_MAP_ITEM_DEF(BM_Hue, SkBlendMode::kHue)
  SWITCH_MAP_ITEM_DEF(BM_Saturation, SkBlendMode::kSaturation)
  SWITCH_MAP_ITEM_DEF(BM_Color, SkBlendMode::kColor)
  SWITCH_MAP_ITEM_DEF(BM_Luminosity, SkBlendMode::kColor)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Plus_darker)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Plus_lighter)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Blend_divide)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Blend_subtraction)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Darker_color)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Dissolve)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Hard_mix)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Lighter_color)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Lighten_burn)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Lighten_dodge)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Lighten_light)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Pass_through)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Pin_Light)
  SWITCH_MAP_ITEM_DEF_NULL(BM_Vivid_light)
  SWITCH_MAP_ITEM_END(SkBlendMode::kSrcOver)
  return SkBlendMode::kSrcOver;
}

inline SkPaint::Cap toSkPaintCap(VGG::ELineCap cap)
{
  SWITCH_MAP_ITEM_BEGIN(cap)
  SWITCH_MAP_ITEM_DEF(LC_Butt, SkPaint::kButt_Cap)
  SWITCH_MAP_ITEM_DEF(LC_Round, SkPaint::kRound_Cap)
  SWITCH_MAP_ITEM_DEF(LC_Square, SkPaint::kSquare_Cap)
  SWITCH_MAP_ITEM_END(SkPaint::kButt_Cap)
}

inline SkPathOp toSkPathOp(VGG::EBoolOp blop)
{
  SWITCH_MAP_ITEM_BEGIN(blop)
  SWITCH_MAP_ITEM_DEF(VGG::BO_Union, SkPathOp::kUnion_SkPathOp)
  SWITCH_MAP_ITEM_DEF(BO_Substraction, SkPathOp::kDifference_SkPathOp)
  SWITCH_MAP_ITEM_DEF(VGG::BO_Intersection, SkPathOp::kIntersect_SkPathOp)
  SWITCH_MAP_ITEM_DEF(VGG::BO_Exclusion, SkPathOp::kXOR_SkPathOp)
  SWITCH_MAP_ITEM_END(SkPathOp::kUnion_SkPathOp)
}

constexpr float EPS = std::numeric_limits<float>::epsilon();

inline double calcRadius(double r0,
                         const glm::vec2& p0,
                         const glm::vec2& p1,
                         const glm::vec2& p2,
                         glm::vec2* left,
                         glm::vec2* right)
{
  glm::vec2 a = p0 - p1;
  glm::vec2 b = p2 - p1;
  double alen = a.length();
  double blen = b.length();
  if (std::fabs(alen) < EPS || std::fabs(blen) < EPS)
  {
    return 0.;
  }
  ASSERT(alen > 0 && blen > 0);
  double cosTheta = glm::dot(a, b) / alen / blen;
  if (cosTheta + 1 < EPS) // cosTheta == -1
  {
    if (left)
    {
      left->x = p1.x;
      left->y = p1.y;
    }
    if (right)
    {
      right->x = p1.x;
      right->y = p1.y;
    }
    return r0;
  }
  else if (1 - cosTheta < EPS) // cosTheta == 1
  {
    return 0.;
  }
  double tanHalfTheta = std::sqrt((1 - cosTheta) / (1 + cosTheta));
  double radius = r0;
  radius = std::min(radius, 0.5 * alen * tanHalfTheta);
  radius = std::min(radius, 0.5 * blen * tanHalfTheta);
  if (left)
  {
    ASSERT(tanHalfTheta > 0);
    float len = radius / tanHalfTheta;
    *left = p1 + float(len / alen) * a;
  }
  if (right)
  {
    ASSERT(tanHalfTheta > 0);
    double len = radius / tanHalfTheta;
    *right = p1 + (float(len / blen) * b);
  }
  return radius;
}

inline SkMatrix upperMatrix22(const SkMatrix& matrix)
{
  SkMatrix m = matrix;
  m.setTranslateX(0);
  m.setTranslateY(1);
  return m;
}

inline SkPath getSkiaPath(const std::vector<PointAttr>& points, bool isClosed)
{
  constexpr float w = 1.0;
  constexpr float h = 1.0;
  auto& pts = points;

  ASSERT(w > 0);
  ASSERT(h > 0);

  SkPath skPath;

  if (pts.size() < 2)
  {
    // WARN("Too few path points.");
    return skPath;
  }

  using PM = EPointMode;
  auto* startP = &pts[0];
  auto* endP = &pts[pts.size() - 1];
  auto* prevP = endP;
  auto* currP = startP;
  auto* nextP = currP + 1;

  const glm::vec2 s = { w, h };

  if (currP->radius > 0 && currP->mode() == PM::PM_Straight)
  {
    glm::vec2 start = currP->point * s;
    calcRadius(currP->radius,
               prevP->point * s,
               currP->point * s,
               nextP->point * s,
               nullptr,
               &start);
    skPath.moveTo(start.x, start.y);
  }
  else
  {
    skPath.moveTo(w * currP->point.x, h * currP->point.y);
  }

  while (true)
  {
    if (currP->mode() == PM::PM_Straight && nextP->mode() == PM::PM_Straight)
    {
      if (nextP->radius > 0 && nextP->mode() == PM::PM_Straight)
      {
        auto* next2P = (nextP == endP) ? startP : (nextP + 1);
        auto next2Pp = next2P->to.has_value() ? next2P->to.value() : next2P->point;
        double r = calcRadius(nextP->radius, currP->point * s, nextP->point * s, next2Pp * s, 0, 0);
        skPath.arcTo(w * nextP->point.x, h * nextP->point.y, w * next2Pp.x, h * next2Pp.y, r);
      }
      else
      {
        skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
      }
    }
    else if (currP->mode() == PM::PM_Disconnected && nextP->mode() == PM::PM_Disconnected)
    {
      bool hasFrom = currP->from.has_value();
      bool hasTo = nextP->to.has_value();
      if (!hasFrom && !hasTo)
      {
        skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
      }
      else if (hasFrom && !hasTo)
      {
        auto& from = currP->from.value();
        skPath.quadTo(w * from.x, h * from.y, w * nextP->point.x, h * nextP->point.y);
      }
      else if (!hasFrom && hasTo)
      {
        auto& to = nextP->to.value();
        skPath.quadTo(w * to.x, h * to.y, w * nextP->point.x, h * nextP->point.y);
      }
      else
      {
        auto& from = currP->from.value();
        auto& to = nextP->to.value();
        skPath.cubicTo(w * from.x,
                       h * from.y,
                       w * to.x,
                       h * to.y,
                       w * nextP->point.x,
                       h * nextP->point.y);
      }
    }
    else if (currP->mode() != PM::PM_Straight && nextP->mode() != PM::PM_Straight)
    {
      if ((currP->mode() == PM::PM_Disconnected && !currP->from.has_value()) ||
          (nextP->mode() == PM::PM_Disconnected && !nextP->to.has_value()) ||
          (currP->mode() != PM::PM_Disconnected &&
           !(currP->from.has_value() && currP->to.has_value())) ||
          (nextP->mode() != PM::PM_Disconnected &&
           !(nextP->from.has_value() && nextP->to.has_value())))
      {
        WARN("Missing control points.");
        return skPath;
      }
      auto& from = currP->from.value();
      auto& to = nextP->to.value();
      skPath.cubicTo(w * from.x,
                     h * from.y,
                     w * to.x,
                     h * to.y,
                     w * nextP->point.x,
                     h * nextP->point.y);
    }
    else if (currP->mode() == PM::PM_Straight && nextP->mode() != PM::PM_Straight)
    {
      if (!nextP->to.has_value())
      {
        skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
      }
      else
      {
        auto& to = nextP->to.value();
        skPath.quadTo(w * to.x, h * to.y, w * nextP->point.x, h * nextP->point.y);
      }
    }
    else if (currP->mode() != PM::PM_Straight && nextP->mode() == PM::PM_Straight)
    {
      if (nextP->radius > 0 && nextP->mode() == PM::PM_Straight)
      {
        auto* next2P = (nextP == endP) ? startP : (nextP + 1);
        if (!currP->from.has_value())
        {
          glm::vec2 start;
          double r = calcRadius(nextP->radius,
                                currP->point * s,
                                nextP->point * s,
                                next2P->point * s,
                                &start,
                                nullptr);
          skPath.lineTo(start.x, start.y);
          skPath.arcTo(w * nextP->point.x,
                       h * nextP->point.y,
                       w * next2P->point.x,
                       h * next2P->point.y,
                       r);
        }
        else
        {
          auto currPfrom = currP->from.value();
          constexpr float radius_coeff = 1.0;
          // glm::vec2 p =
          // currP->point.add(currPfrom.sub(currP->point).scale(radius_coeff)).scale(w, h);
          glm::vec2 p = (currP->point + radius_coeff * (currPfrom - currP->point)) * s;
          glm::vec2 start;
          double r =
            calcRadius(nextP->radius, p, nextP->point * s, next2P->point * s, &start, nullptr);
          skPath.quadTo(p.x, p.y, start.x, start.y);
          skPath.arcTo(w * nextP->point.x,
                       h * nextP->point.y,
                       w * next2P->point.x,
                       h * next2P->point.y,
                       r);
        }
      }
      else
      {
        if (!currP->from.has_value())
        {
          skPath.lineTo(w * nextP->point.x, h * nextP->point.y);
        }
        else
        {
          auto& from = currP->from.value();
          skPath.quadTo(w * from.x, h * from.y, w * nextP->point.x, h * nextP->point.y);
        }
      }
    }
    else
    {
      WARN("Invalid point mode combination: %d %d", (int)currP->mode(), (int)nextP->mode());
    }
    currP = nextP;
    nextP = (nextP == endP) ? startP : (nextP + 1);

    if (isClosed)
    {
      if (currP == startP)
      {
        break;
      }
    }
    else
    {
      if (nextP == startP)
      {
        break;
      }
    }
  }

  if (isClosed)
  {
    skPath.close();
  }

  return skPath;
}

inline sk_sp<SkShader> getImageShader(sk_sp<SkImage> img,
                                      int width,
                                      int height,
                                      EImageFillType imageFillType,
                                      float imageTileScale,
                                      bool imageTileMirrored,
                                      const SkMatrix* matrix = nullptr)
{

  SkTileMode modeX = SkTileMode::kDecal;
  SkTileMode modeY = SkTileMode::kDecal;
  SkMatrix mat = SkMatrix::I();
  if (matrix)
  {
    mat.postConcat(*matrix);
  }
  SkImageInfo mi = img->imageInfo();
  float sx = (float)width / mi.width();
  float sy = (float)height / mi.height();
  if (imageFillType == IFT_Fill)
  {
    double s = std::max(sx, sy);
    mat.postScale(s, s);
    if (matrix)
    {
      mat.postConcat(upperMatrix22(*matrix));
    }
    if (sx > sy)
    {
      // scaled image's width == frame's width
      mat.postTranslate(0, (height - sx * mi.height()) / 2);
    }
    else
    {
      // scaled image's height == frame's height
      mat.postTranslate((width - sy * mi.width()) / 2, 0);
    }
  }
  else if (imageFillType == IFT_Fit)
  {

    if (matrix)
    {
      mat.postConcat(upperMatrix22(*matrix));
    }
    double s = std::min(sx, sy);
    mat.postScale(s, s);
    if (matrix)
    {
      mat.postConcat(*matrix);
    }
    if (sx < sy)
    {
      // scaled image's width == frame's width
      mat.postTranslate(0, (height - sx * mi.height()) / 2);
    }
    else
    {
      // scaled image's height == frame's height
      mat.postTranslate((width - sy * mi.width()) / 2, 0);
    }
  }
  else if (imageFillType == VGG::IFT_Stretch)
  {
    if (matrix)
    {
      mat.postConcat(upperMatrix22(*matrix));
    }
    mat.postScale(sx, sy);
  }
  else if (imageFillType == VGG::IFT_Tile)
  {
    modeX = imageTileMirrored ? SkTileMode::kMirror : SkTileMode::kRepeat;
    modeY = imageTileMirrored ? SkTileMode::kMirror : SkTileMode::kRepeat;
    if (matrix)
    {
      mat.postConcat(*matrix);
    }
  }
  else if (imageFillType == IFT_OnlyTileVertical)
  {
    if (matrix)
    {
      mat.postConcat(*matrix);
    }
    modeY = imageTileMirrored ? SkTileMode::kMirror : SkTileMode::kRepeat;
  }
  else if (imageFillType == IFT_OnlyTileHorizontal)
  {
    modeX = imageTileMirrored ? SkTileMode::kMirror : SkTileMode::kRepeat;
  }
  SkSamplingOptions opt;
  mat.postScale(1, -1); // convert to skia

  return img->makeShader(modeX, modeY, opt, &mat);
}

inline sk_sp<SkImage> loadImage(const std::string& imageGUID, const ResourceRepo& repo)
{
  sk_sp<SkImage> image;
  if (imageGUID.empty())
    return image;
  std::string guid = imageGUID;
  if (auto pos = guid.find("./"); pos != std::string::npos && pos == 0)
  {
    // remove current dir notation
    guid = guid.substr(2);
  }
  if (auto it = SkiaImageRepo.find(guid); it != SkiaImageRepo.end())
  {
    image = it->second;
  }
  else
  {
    auto repo = Scene::getResRepo();
    if (auto it = repo.find(guid); it != repo.end())
    {
      auto data = SkData::MakeWithCopy(it->second.data(), it->second.size());
      if (!data)
      {
        WARN("Make SkData failed");
        return image;
      }
      sk_sp<SkImage> skImage = SkImage::MakeFromEncoded(data);
      if (!skImage)
      {
        WARN("Make SkImage failed.");
        return image;
      }
      SkiaImageRepo[guid] = skImage;
      image = skImage;
    }
    else
    {
      WARN("Cannot find %s from resources repository", guid.c_str());
    }
  }
  return image;
}