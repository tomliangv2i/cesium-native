// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Cesium3DTiles/GroupMetadata.h"
#include "Cesium3DTiles/Library.h"
#include "Cesium3DTiles/Schema.h"
#include "Cesium3DTiles/Statistics.h"
#include "Cesium3DTiles/TilesetMetadata.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <optional>
#include <string>
#include <vector>

namespace Cesium3DTiles {
/**
 * @brief 3D Tiles extension that defines metadata for a tileset.
 */
struct CESIUM3DTILES_API ExtensionTileset3dTilesMetadata final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName =
      "ExtensionTileset3dTilesMetadata";
  static inline constexpr const char* ExtensionName = "3DTILES_metadata";

  /**
   * @brief An object defining classes and enums.
   */
  std::optional<Cesium3DTiles::Schema> schema;

  /**
   * @brief The URI (or IRI) of the external schema file.
   */
  std::optional<std::string> schemaUri;

  /**
   * @brief An object containing statistics about entities.
   */
  std::optional<Cesium3DTiles::Statistics> statistics;

  /**
   * @brief An array of groups.
   */
  std::vector<Cesium3DTiles::GroupMetadata> groups;

  /**
   * @brief An object containing metadata about the tileset.
   */
  std::optional<Cesium3DTiles::TilesetMetadata> tileset;
};
} // namespace Cesium3DTiles
