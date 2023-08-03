#include "include/core/SkFontArguments.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkStream.h"
#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkTypes.h"
#include "include/private/base/SkTArray.h"
#include "include/private/base/SkTemplates.h"
#include "src/core/SkFontDescriptor.h"
#include "SkiaImpl/VSkFontMgr.h"
#include "rapidfuzz/fuzz.hpp"

#include <limits>
#include <memory>

using namespace skia_private;

class SkData;

SkTypeface_VGG::SkTypeface_VGG(const SkFontStyle& style,
                               bool isFixedPitch,
                               bool sysFont,
                               const SkString familyName,
                               int index)
  : INHERITED(style, isFixedPitch)
  , fIsSysFont(sysFont)
  , fFamilyName(familyName)
  , fIndex(index)
{
}

bool SkTypeface_VGG::isSysFont() const
{
  return fIsSysFont;
}

void SkTypeface_VGG::onGetFamilyName(SkString* familyName) const
{
  *familyName = fFamilyName;
}

void SkTypeface_VGG::onGetFontDescriptor(SkFontDescriptor* desc, bool* isLocal) const
{
  desc->setFamilyName(fFamilyName.c_str());
  desc->setStyle(this->fontStyle());
  desc->setFactoryId(SkTypeface_FreeType::FactoryId);
  *isLocal = !this->isSysFont();
}

int SkTypeface_VGG::getIndex() const
{
  return fIndex;
}

SkTypeface_VGG_Empty::SkTypeface_VGG_Empty()
  : INHERITED(SkFontStyle(), false, true, SkString(), 0)
{
}

std::unique_ptr<SkStreamAsset> SkTypeface_VGG_Empty::onOpenStream(int*) const
{
  return nullptr;
}

sk_sp<SkTypeface> SkTypeface_VGG_Empty::onMakeClone(const SkFontArguments& args) const
{
  return sk_ref_sp(this);
}

std::unique_ptr<SkFontData> SkTypeface_VGG_Empty::onMakeFontData() const
{
  return nullptr;
}

SkTypeface_VGG_File::SkTypeface_VGG_File(const SkFontStyle& style,
                                         bool isFixedPitch,
                                         bool sysFont,
                                         const SkString familyName,
                                         const char path[],
                                         int index)
  : INHERITED(style, isFixedPitch, sysFont, familyName, index)
  , fPath(path)
{
}

std::unique_ptr<SkStreamAsset> SkTypeface_VGG_File::onOpenStream(int* ttcIndex) const
{
  *ttcIndex = this->getIndex();
  return SkStream::MakeFromFile(fPath.c_str());
}

sk_sp<SkTypeface> SkTypeface_VGG_File::onMakeClone(const SkFontArguments& args) const
{
  std::unique_ptr<SkFontData> data = this->cloneFontData(args);
  if (!data)
  {
    return nullptr;
  }

  SkString familyName;
  this->getFamilyName(&familyName);

  return sk_make_sp<SkTypeface_FreeTypeStream>(std::move(data),
                                               familyName,
                                               this->fontStyle(),
                                               this->isFixedPitch());
}

std::unique_ptr<SkFontData> SkTypeface_VGG_File::onMakeFontData() const
{
  int index;
  std::unique_ptr<SkStreamAsset> stream(this->onOpenStream(&index));
  if (!stream)
  {
    return nullptr;
  }
  return std::make_unique<SkFontData>(std::move(stream), index, 0, nullptr, 0, nullptr, 0);
}

///////////////////////////////////////////////////////////////////////////////

SkFontStyleSet_VGG::SkFontStyleSet_VGG(const SkString familyName)
  : fFamilyName(familyName)
{
}

void SkFontStyleSet_VGG::appendTypeface(sk_sp<SkTypeface> typeface)
{
  fStyles.emplace_back(std::move(typeface));
}

int SkFontStyleSet_VGG::count()
{
  return fStyles.size();
}

void SkFontStyleSet_VGG::getStyle(int index, SkFontStyle* style, SkString* name)
{
  SkASSERT(index < fStyles.size());
  if (style)
  {
    *style = fStyles[index]->fontStyle();
  }
  if (name)
  {
    name->reset();
  }
}

sk_sp<SkTypeface> SkFontStyleSet_VGG::createTypeface(int index)
{
  SkASSERT(index < fStyles.size());
  return fStyles[index];
}

sk_sp<SkTypeface> SkFontStyleSet_VGG::matchStyle(const SkFontStyle& pattern)
{
  return this->matchStyleCSS3(pattern);
}

SkString SkFontStyleSet_VGG::getFamilyName()
{
  return fFamilyName;
}

SkFontMgrVGG::SkFontMgrVGG(std::unique_ptr<SystemFontLoader> loader)
  : fDefaultFamily(nullptr)
  , m_loader(std::move(loader))
{
  m_loader->loadSystemFonts(fScanner, &fFamilies);

  // Try to pick a default font.
  static const char* defaultNames[] = { "Arial",      "Verdana",      "Times New Roman",
                                        "Droid Sans", "DejaVu Serif", nullptr };
  for (size_t i = 0; i < std::size(defaultNames); ++i)
  {
    sk_sp<SkFontStyleSet> set(this->onMatchFamily(defaultNames[i]));
    if (nullptr == set)
    {
      continue;
    }

    sk_sp<SkTypeface> tf(set->matchStyle(SkFontStyle(SkFontStyle::kNormal_Weight,
                                                     SkFontStyle::kNormal_Width,
                                                     SkFontStyle::kUpright_Slant)));
    if (nullptr == tf)
    {
      continue;
    }

    fDefaultFamily = set;
    break;
  }
  if (nullptr == fDefaultFamily)
  {
    fDefaultFamily = fFamilies[0];
  }
}

int SkFontMgrVGG::onCountFamilies() const
{
  return fFamilies.size();
}

void SkFontMgrVGG::onGetFamilyName(int index, SkString* familyName) const
{
  SkASSERT(index < fFamilies.size());
  familyName->set(fFamilies[index]->getFamilyName());
}

sk_sp<SkFontStyleSet> SkFontMgrVGG::onCreateStyleSet(int index) const
{
  SkASSERT(index < fFamilies.size());
  return fFamilies[index];
}

sk_sp<SkFontStyleSet> SkFontMgrVGG::onMatchFamily(const char familyName[]) const
{
  // for (int i = 0; i < fFamilies.size(); ++i) {
  //     if (fFamilies[i]->getFamilyName().equals(familyName)) {
  //         return fFamilies[i];
  //     }
  // }
  if (!familyName)
    return nullptr;
  if (auto it = fFamilies.lookUp.find(familyName); it != fFamilies.lookUp.end())
  {
    return fFamilies.fFamilies[it->second];
  }
  return nullptr;
}

sk_sp<SkTypeface> SkFontMgrVGG::onMatchFamilyStyle(const char familyName[],
                                                   const SkFontStyle& fontStyle) const
{
  sk_sp<SkFontStyleSet> sset(this->matchFamily(familyName));
  return sset->matchStyle(fontStyle);
}

sk_sp<SkTypeface> SkFontMgrVGG::onMatchFamilyStyleCharacter(const char familyName[],
                                                            const SkFontStyle&,
                                                            const char* bcp47[],
                                                            int bcp47Count,
                                                            SkUnichar) const
{
  return nullptr;
}

sk_sp<SkTypeface> SkFontMgrVGG::onMakeFromData(sk_sp<SkData> data, int ttcIndex) const
{
  return this->makeFromStream(std::make_unique<SkMemoryStream>(std::move(data)), ttcIndex);
}

sk_sp<SkTypeface> SkFontMgrVGG::onMakeFromStreamIndex(std::unique_ptr<SkStreamAsset> stream,
                                                      int ttcIndex) const
{
  return this->makeFromStream(std::move(stream), SkFontArguments().setCollectionIndex(ttcIndex));
}

sk_sp<SkTypeface> SkFontMgrVGG::onMakeFromStreamArgs(std::unique_ptr<SkStreamAsset> stream,
                                                     const SkFontArguments& args) const
{
  return SkTypeface_FreeType::MakeFromStream(std::move(stream), args);
}

sk_sp<SkTypeface> SkFontMgrVGG::onMakeFromFile(const char path[], int ttcIndex) const
{
  std::unique_ptr<SkStreamAsset> stream = SkStream::MakeFromFile(path);
  return stream ? this->makeFromStream(std::move(stream), ttcIndex) : nullptr;
}

std::pair<SkString, float> SkFontMgrVGG::fuzzyMatchFontFamilyName(const std::string& fontName) const
{
  bool match_found = false;
  double best_score = 0.0;
  SkString best_match;
  rapidfuzz::fuzz::CachedRatio<char> scorer(fontName);
  for (const auto& style : fFamilies.fFamilies)
  {
    auto choice = std::string(style->getFamilyName().c_str());
    double score = scorer.similarity(choice, best_score);

    if (score >= best_score)
    {
      match_found = true;
      best_score = score;
      best_match = style->getFamilyName();
    }
  }
  return { best_match, best_score };
}

sk_sp<SkTypeface> SkFontMgrVGG::onLegacyMakeTypeface(const char familyName[],
                                                     SkFontStyle style) const
{
  sk_sp<SkTypeface> tf;

  if (familyName)
  {
    tf = this->onMatchFamilyStyle(familyName, style);
  }

  if (!tf)
  {
    tf = fDefaultFamily->matchStyle(style);
  }

  return tf;
}