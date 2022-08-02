vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO invor/glowl
  REF v0.4g
  SHA512 2d46f44b302cd05455b25a1b4b5438194aa387cf43f392ee62558e5d02fc4751695a054a13dfcec60a2cb975670a54c4783085fbbe29068f5302d1ce49d2960f
  HEAD_REF master
)

vcpkg_cmake_configure(
  SOURCE_PATH ${SOURCE_PATH}
  OPTIONS
    -DGLOWL_OPENGL_INCLUDE=GLAD2
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/glowl)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug" "${CURRENT_PACKAGES_DIR}/lib")
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
