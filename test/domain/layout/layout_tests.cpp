#include "Domain/Layout/Layout.hpp"
#include "UseCase/StartRunning.hpp"

#include "domain/model/daruma_helper.hpp"

#include <gtest/gtest.h>

using namespace VGG;

class VggLayoutTestSuite : public ::testing::Test
{
protected:
  std::shared_ptr<Layout::Layout> m_sut;

  void SetUp() override
  {
  }

  void TearDown() override
  {
    m_sut.reset();
  }
};

TEST_F(VggLayoutTestSuite, Smoke)
{
  // Given
  std::shared_ptr<Daruma> daruma{ new Daruma(Helper::RawJsonDocumentBuilder,
                                             Helper::RawJsonDocumentBuilder) };
  std::string filePath = "testDataDir/layout/0_space_between/";
  daruma->load(filePath);

  // When
  Layout::Layout sut{ daruma->designDoc(), daruma->layoutDoc() };

  // Then
}

TEST_F(VggLayoutTestSuite, Layout)
{
  // Given
  std::shared_ptr<Daruma> daruma{ new Daruma(Helper::RawJsonDocumentBuilder,
                                             Helper::RawJsonDocumentBuilder) };
  std::string filePath = "testDataDir/layout/0_space_between/";
  daruma->load(filePath);

  Layout::Layout sut{ daruma->designDoc(), daruma->layoutDoc() };
  Layout::Size windowSize{ 1400, 900 };

  // When
  sut.layout(windowSize);

  // Then
  auto tree = sut.layoutTree();
  auto currentPage = tree->children()[0];

  auto leftChildFrame = currentPage->children()[0]->frame();
  Layout::Rect expectedLeftChildFrame{ { 100, 40 }, { 200, 150 } };
  EXPECT_TRUE(leftChildFrame == expectedLeftChildFrame);

  auto rightChildFrame = currentPage->children()[1]->frame();
  Layout::Rect expectedRightChildFrame{ { 1100, 40 }, { 200, 250 } };
  EXPECT_TRUE(rightChildFrame == expectedRightChildFrame);
}

TEST_F(VggLayoutTestSuite, GridWrap)
{
  // Given
  std::shared_ptr<Daruma> daruma{ new Daruma(Helper::RawJsonDocumentBuilder,
                                             Helper::RawJsonDocumentBuilder) };
  std::string filePath = "testDataDir/layout/1_grid_wrap/";
  daruma->load(filePath);

  Layout::Layout sut{ daruma->designDoc(), daruma->layoutDoc() };
  Layout::Size windowSize{ 1400, 900 };

  // When
  sut.layout(windowSize);

  // Then
  auto tree = sut.layoutTree();
  auto currentPage = tree->children()[0];

  auto gridContainer = currentPage;

  auto child0Frame = gridContainer->children()[0]->frame();
  Layout::Rect expectChild0Frame{ { 100, 40 }, { 200, 250 } };
  EXPECT_TRUE(child0Frame == expectChild0Frame);

  auto child3Frame = gridContainer->children()[3]->frame();
  Layout::Rect expectChild3Frame{ { 1060, 40 }, { 200, 200 } };
  EXPECT_TRUE(child3Frame == expectChild3Frame);
}

TEST_F(VggLayoutTestSuite, ScaleSubtree)
{
  // Given
  std::shared_ptr<Daruma> daruma{ new Daruma(Helper::RawJsonDocumentBuilder,
                                             Helper::RawJsonDocumentBuilder) };
  std::string filePath = "testDataDir/layout/3_flex_with_symbol_instance/";
  daruma->load(filePath);

  StartRunning startRunning{ daruma };
  auto sut = startRunning.layout();
  Layout::Size windowSize{ 1400, 900 };

  // When
  sut->layout(windowSize);

  // Then
  auto tree = sut->layoutTree();
  auto currentPage = tree->children()[1];

  auto container = currentPage->children()[0];

  auto child0Frame = container->children()[0]->frame();
  Layout::Rect expectChild0Frame{ { 0, 0 }, { 947, 839.999938 } };
  EXPECT_TRUE(child0Frame == expectChild0Frame);

  auto child1Frame = container->children()[1]->frame();
  Layout::Rect expectChild1Frame{ { 104.170006, 188.999985 }, { 130.685989, 361.199982 } };
  EXPECT_TRUE(child1Frame == expectChild1Frame);
}