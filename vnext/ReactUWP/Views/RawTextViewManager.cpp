// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "RawTextViewManager.h"
#include "TextViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <INativeUIManager.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {

RawTextViewManager::RawTextViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *RawTextViewManager::GetName() const {
  return "RCTRawText";
}

XamlView RawTextViewManager::CreateViewCore(int64_t tag) {
  winrt::Run run;
  return run;
}

void RawTextViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto run = nodeToUpdate->GetView().as<winrt::Run>();
  if (run == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "text") {
      run.Text(asHstring(propertyValue));
      if (nodeToUpdate->GetParent() != -1) {
        if (auto instance = this->m_wkReactInstance.lock()) {
          const ShadowNodeBase *parent = static_cast<ShadowNodeBase *>(
              instance->NativeUIManager()->getHost()->FindShadowNodeForTag(
                  nodeToUpdate->GetParent()));
          if (parent && parent->m_children.size() == 1) {
            auto view = parent->GetView();
            auto textBlock = view.try_as<winrt::TextBlock>();
            if (textBlock != nullptr) {
              textBlock.Text(run.Text());
            }
          }

          NotifyAncestorsTextChanged(instance.operator->(), nodeToUpdate);
        }
      }
    }
  }
  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void RawTextViewManager::NotifyAncestorsTextChanged(
    IReactInstance *instance,
    ShadowNodeBase *nodeToUpdate) {
  auto host = instance->NativeUIManager()->getHost();
  ShadowNodeBase *parent = static_cast<ShadowNodeBase *>(
      host->FindShadowNodeForTag(nodeToUpdate->GetParent()));
  while (parent) {
    auto viewManager = parent->GetViewManager();
    if (!std::strcmp(viewManager->GetName(), "RCTText")) {
      (static_cast<TextViewManager *>(viewManager))
          ->OnDescendantTextPropertyChanged(parent);
    }
    parent = static_cast<ShadowNodeBase *>(
        host->FindShadowNodeForTag(parent->GetParent()));
  }
}

void RawTextViewManager::SetLayoutProps(
    ShadowNodeBase &nodeToUpdate,
    XamlView viewToUpdate,
    float left,
    float top,
    float width,
    float height) {}

bool RawTextViewManager::RequiresYogaNode() const {
  return false;
}

} // namespace uwp
} // namespace react
