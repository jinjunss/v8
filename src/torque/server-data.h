// Copyright 2019 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_TORQUE_SERVER_DATA_H_
#define V8_TORQUE_SERVER_DATA_H_

#include <map>
#include <vector>

#include "src/base/macros.h"
#include "src/base/optional.h"
#include "src/torque/declarable.h"
#include "src/torque/global-context.h"
#include "src/torque/source-positions.h"

namespace v8 {
namespace internal {
namespace torque {

// The definition of the token in the first element, can be found at the second.
using DefinitionMapping = std::pair<SourcePosition, SourcePosition>;
// TODO(szuend): Support overlapping source positions when we start adding them.
using Definitions = std::vector<DefinitionMapping>;
using DefinitionsMap = std::map<SourceId, Definitions>;

// Symbols are used to answer search queries (either workspace or document
// scope). For now, declarables are stored directly without converting them
// into a custom format. Symbols are grouped by sourceId to implement document
// scoped searches.
using Symbols = std::vector<Declarable*>;
using SymbolsMap = std::map<SourceId, Symbols>;

// This contextual class holds all the necessary data to answer incoming
// LSP requests. It is reset for each compilation step and all information
// is calculated eagerly during compilation.
class LanguageServerData : public ContextualClass<LanguageServerData> {
 public:
  LanguageServerData() = default;

  V8_EXPORT_PRIVATE static void AddDefinition(SourcePosition token,
                                              SourcePosition definition);

  V8_EXPORT_PRIVATE static base::Optional<SourcePosition> FindDefinition(
      SourceId source, LineAndColumn pos);

  static void SetGlobalContext(GlobalContext global_context) {
    Get().global_context_ =
        base::make_unique<GlobalContext>(std::move(global_context));
    Get().PrepareAllDeclarableSymbols();
  }

  static const Symbols& SymbolsForSourceId(SourceId id) {
    return Get().symbols_map_[id];
  }

 private:
  // Splits all declarables up by SourceId and filters out auto-generated ones.
  void PrepareAllDeclarableSymbols();

  DefinitionsMap definitions_map_;
  SymbolsMap symbols_map_;
  std::unique_ptr<GlobalContext> global_context_;
};

}  // namespace torque
}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_SERVER_DATA_H_
