#pragma once

#include "BoundingVolume.h"
#include "Library.h"
#include "TileContentLoadResult.h"
#include "TileContentLoader.h"
#include "TileID.h"
#include "TileRefine.h"

#include <CesiumAsync/AsyncSystem.h>
#include <CesiumAsync/Future.h>
#include <CesiumAsync/HttpHeaders.h>
#include <CesiumAsync/IAssetAccessor.h>
#include <CesiumGltfReader/GltfReader.h>

#include <glm/mat4x4.hpp>
#include <gsl/span>
#include <spdlog/fwd.h>

#include <cstddef>
#include <optional>

namespace CesiumGeospatial {
class GlobeRectangle;
}

namespace Cesium3DTilesSelection {

class Tileset;

/**
 * @brief Creates {@link TileContentLoadResult} from glTF data.
 */
class CESIUM3DTILESSELECTION_API GltfContent final : public TileContentLoader {
public:
  /**
   * @copydoc TileContentLoader::load
   *
   * The result will only contain the `model`. Other fields will be
   * empty or have default values.
   */
  CesiumAsync::Future<std::unique_ptr<TileContentLoadResult>>
  load(const TileContentLoadInput& input) override;

  /**
   * @brief Create a {@link TileContentLoadResult} from the given data.
   *
   * (Only public to be called from `Batched3DModelContent`)
   *
   * @param asyncSystem The async system to use for requesting any external
   * content.
   * @param pLogger Only used for logging
   * @param url The URL, only used for logging
   * @param headers The http headers to use for resolving any external content.
   * @param pAssetAccessor The asset accessor to use to resolve external
   * content.
   * @param data The actual glTF data
   * @param contentOptions The content options for loading this glTF.
   * @return The {@link TileContentLoadResult}
   */
  static CesiumAsync::Future<std::unique_ptr<TileContentLoadResult>> load(
      const CesiumAsync::AsyncSystem& asyncSystem,
      const std::shared_ptr<spdlog::logger>& pLogger,
      const std::string& url,
      const CesiumAsync::HttpHeaders& headers,
      const std::shared_ptr<CesiumAsync::IAssetAccessor>& pAssetAccessor,
      const gsl::span<const std::byte>& data,
      const TilesetContentOptions& contentOptions);

  /**
   * @brief Creates texture coordinates for mapping {@link RasterOverlay} tiles
   * to {@link Tileset} tiles.
   *
   * Generates new texture coordinates for the `gltf` using the given
   * `projections`. The first new texture coordinate (`u` or `s`) will be 0.0 at
   * the `minimumX` of the given `rectangle` and 1.0 at the `maximumX`. The
   * second texture coordinate (`v` or `t`) will be 0.0 at the `minimumY` of
   * the given `rectangle` and 1.0 at the `maximumY`.
   *
   * Coordinate values for vertices in between these extremes are determined by
   * projecting the vertex position with the `projection` and then computing the
   * fractional distance of that projected position between the minimum and
   * maximum.
   *
   * Projected positions that fall outside the `globeRectangle` will be clamped
   * to the edges, so the coordinate values will never be less then 0.0 or
   * greater than 1.0.
   *
   * These texture coordinates are stored in the provided glTF, and a new
   * primitive attribute named `_CESIUMOVERLAY_n` is added to each primitive,
   * where `n` starts with the `firstTextureCoordinateID` passed to this
   * function and increases with each projection.
   *
   * @param gltf The glTF model.
   * @param modelToEcefTransform The transformation of this glTF to ECEF
   * coordinates.
   * @param firstTextureCoordinateID The texture coordinate ID of the first
   * projection.
   * @param globeRectangle The rectangle that all projected vertex positions are
   * expected to lie within. If this parameter is std::nullopt, it is computed
   * from the vertices.
   * @param projections The projections for which to generate texture
   * coordinates. There is a linear relationship between the coordinates of this
   * projection and the generated texture coordinates.
   * @return The detailed of the generated texture coordinates.
   */
  static std::optional<TileContentDetailsForOverlays>
  createRasterOverlayTextureCoordinates(
      CesiumGltf::Model& gltf,
      const glm::dmat4& modelToEcefTransform,
      int32_t firstTextureCoordinateID,
      const std::optional<CesiumGeospatial::GlobeRectangle>& globeRectangle,
      std::vector<CesiumGeospatial::Projection>&& projections);

  /**
   * @brief Computes a bounding region from the vertex positions in a glTF
   * model.
   *
   * If the glTF model spans the anti-meridian, the west and east longitude
   * values will be in the usual -PI to PI range, but east will have a smaller
   * value than west.
   *
   * @param gltf The model.
   * @param transform The transform from model coordinates to ECEF coordinates.
   * @return The computed bounding region.
   */
  static CesiumGeospatial::BoundingRegion computeBoundingRegion(
      const CesiumGltf::Model& gltf,
      const glm::dmat4& transform);

  /**
   * @brief Applies the glTF's RTC_CENTER, if any, to the given rootTransform.
   *
   * @param gltf
   * @param rootTransform
   * @return glm::dmat4x4
   */

  /**
   * @brief Applies the glTF's RTC_CENTER, if any, to the given transform.
   *
   * If the glTF has a `CESIUM_RTC` extension, this function will multiply the
   * given matrix with the (translation) matrix that is created from the
   * `RTC_CENTER` in the. If the given model does not have this extension, then
   * this function will return the `rootTransform` unchanged.
   *
   * @param model The glTF model
   * @param rootTransform The matrix that will be multiplied with the transform
   * @return The result of multiplying the `RTC_CENTER` with the
   * `rootTransform`.
   */
  static glm::dmat4x4 applyRtcCenter(
      const CesiumGltf::Model& gltf,
      const glm::dmat4x4& rootTransform);

  /**
   * @brief Applies the glTF's `gltfUpAxis`, if any, to the given transform.
   *
   * By default, the up-axis of a glTF model will the the Y-axis.
   *
   * If the tileset that contained the model had the `asset.gltfUpAxis` string
   * property, then the information about the up-axis has been stored in as a
   * number property called `gltfUpAxis` in the `extras` of the given model.
   *
   * Depending on whether this value is `CesiumGeometry::Axis::X`, `Y`, or `Z`,
   * the given matrix will be multiplied with a matrix that converts the
   * respective axis to be the Z-axis, as required by the 3D Tiles standard.
   *
   * @param model The glTF model
   * @param rootTransform The matrix that will be multiplied with the transform
   * @return The result of multiplying the `rootTransform` with the
   * `gltfUpAxis`.
   */
  static glm::dmat4x4 applyGltfUpAxisTransform(
      const CesiumGltf::Model& model,
      const glm::dmat4x4& rootTransform);

private:
  static CesiumGltfReader::GltfReader _gltfReader;
};

} // namespace Cesium3DTilesSelection
