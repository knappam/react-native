/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <react/renderer/components/scrollview/ScrollContentViewShadowNode.h>

#include <gtest/gtest.h>

namespace facebook::react {

namespace {

using yoga::Edge;
using yoga::StyleLength;

ScrollContentViewState stateWith(
    EdgeInsets safeAreaPadding,
    LayoutDirection layoutDirection = LayoutDirection::LeftToRight,
    bool swapLeftAndRightInRTL = false) {
  ScrollContentViewState state;
  state.safeAreaPadding = safeAreaPadding;
  state.layoutDirection = layoutDirection;
  state.swapLeftAndRightInRTL = swapLeftAndRightInRTL;
  return state;
}

} // namespace

TEST(ScrollContentViewShadowNodeTest, leavesStyleWithoutInsets) {
  yoga::Style style;
  style.setPadding(Edge::Left, StyleLength::points(8));

  ScrollContentViewShadowNode::padForSafeArea(style, stateWith({}));

  EXPECT_EQ(style.padding(Edge::Left), StyleLength::points(8));
  EXPECT_FALSE(style.padding(Edge::Start).isDefined());
}

TEST(ScrollContentViewShadowNodeTest, padsRowOnTopOfResolvedPadding) {
  yoga::Style style;
  style.setPadding(Edge::Horizontal, StyleLength::points(16));

  ScrollContentViewShadowNode::padForSafeArea(
      style, stateWith({.left = 0, .top = 0, .right = 84, .bottom = 0}));

  EXPECT_EQ(style.padding(Edge::Start), StyleLength::points(16));
  EXPECT_EQ(style.padding(Edge::End), StyleLength::points(100));
  EXPECT_FALSE(style.padding(Edge::Left).isDefined());
  EXPECT_FALSE(style.padding(Edge::Right).isDefined());
}

TEST(ScrollContentViewShadowNodeTest, prefersLogicalOverPhysicalEdge) {
  yoga::Style style;
  style.setPadding(Edge::Left, StyleLength::points(4));
  style.setPadding(Edge::Start, StyleLength::points(12));

  ScrollContentViewShadowNode::padForSafeArea(
      style, stateWith({.left = 20, .top = 0, .right = 0, .bottom = 0}));

  EXPECT_EQ(style.padding(Edge::Start), StyleLength::points(32));
}

TEST(ScrollContentViewShadowNodeTest, resolvesPercentageToZeroOnInsetEdge) {
  yoga::Style style;
  style.setPadding(Edge::Horizontal, StyleLength::percent(10));

  ScrollContentViewShadowNode::padForSafeArea(
      style, stateWith({.left = 20, .top = 0, .right = 0, .bottom = 0}));

  EXPECT_EQ(style.padding(Edge::Start), StyleLength::points(20));
  EXPECT_EQ(style.padding(Edge::End), StyleLength::percent(10));
}

TEST(ScrollContentViewShadowNodeTest, padsPhysicalEdgesInRTL) {
  yoga::Style style;
  style.setPadding(Edge::Start, StyleLength::points(12));

  ScrollContentViewShadowNode::padForSafeArea(
      style,
      stateWith(
          {.left = 0, .top = 0, .right = 84, .bottom = 0},
          LayoutDirection::RightToLeft));

  // In RTL the start edge is on the right, where the inset is.
  EXPECT_EQ(style.padding(Edge::Start), StyleLength::points(96));
  EXPECT_FALSE(style.padding(Edge::End).isDefined());
}

TEST(ScrollContentViewShadowNodeTest, readsSwappedPhysicalEdgesInRTL) {
  yoga::Style style;
  style.setPadding(Edge::Left, StyleLength::points(8));
  style.setPadding(Edge::End, StyleLength::points(2));

  ScrollContentViewShadowNode::padForSafeArea(
      style,
      stateWith(
          {.left = 84, .top = 0, .right = 0, .bottom = 0},
          LayoutDirection::RightToLeft,
          true));

  // Left is copied over start at layout, so it lands on the right, and the
  // inset on the left adds to the end edge.
  EXPECT_EQ(style.padding(Edge::Start), StyleLength::points(8));
  EXPECT_EQ(style.padding(Edge::End), StyleLength::points(86));
  EXPECT_FALSE(style.padding(Edge::Left).isDefined());
}

TEST(ScrollContentViewShadowNodeTest, padsColumnForHorizontalScrollView) {
  yoga::Style style;
  style.setPadding(Edge::Vertical, StyleLength::points(4));

  ScrollContentViewShadowNode::padForSafeArea(
      style, stateWith({.left = 0, .top = 20, .right = 0, .bottom = 34}));

  EXPECT_EQ(style.padding(Edge::Top), StyleLength::points(24));
  EXPECT_EQ(style.padding(Edge::Bottom), StyleLength::points(38));
  EXPECT_EQ(style.padding(Edge::Vertical), StyleLength::points(4));
}

} // namespace facebook::react
