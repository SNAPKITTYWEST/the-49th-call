import Lake
open Lake DSL

package «AramaicInvariant»

lean_lib «AramaicInvariant»

@[default_target]
lean_exe test_runner where
  root := `Tests.Main

lean_exe parser where
  root := `Internal.Parser

lean_exe collision_search where
  root := `Tests.CollisionSearch
