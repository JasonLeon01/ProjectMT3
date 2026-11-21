"""Regenerate the embedded Vulkan 1.0 shaders using Khronos glslang."""

import argparse
import pathlib
import struct
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("glslang", help="Path to glslang or glslangValidator")
    args = parser.parse_args()

    folder = pathlib.Path(__file__).resolve().parents[1] / "src/SFML/Graphics/Vulkan"
    output = [
        "// Generated from default.vert and default.frag by tools/generate_vulkan_shaders.py.",
        "#pragma once",
        "",
        "#include <array>",
        "",
        "#include <cstdint>",
        "",
        "namespace sf::priv::VulkanShaders",
        "{",
        "// clang-format off",
    ]
    with tempfile.TemporaryDirectory() as temp:
        for name, stage in (("vertex", "vert"), ("fragment", "frag")):
            compiled = pathlib.Path(temp) / "default.spv"
            subprocess.run(
                [args.glslang, "-V", "--target-env", "vulkan1.0", "-o", str(compiled), str(folder / f"default.{stage}")],
                check=True,
            )
            words = [f"0x{word:08x}u" for (word,) in struct.iter_unpack("<I", compiled.read_bytes())]
            output.append(f"inline constexpr std::array<std::uint32_t, {len(words)}> {name}{{")
            output.extend("    " + ", ".join(words[i:i + 6]) + "," for i in range(0, len(words), 6))
            output.extend(["};", ""])
    output.extend(["// clang-format on", "} // namespace sf::priv::VulkanShaders", ""])
    (folder / "BuiltInShaders.hpp").write_text("\n".join(output), encoding="utf-8", newline="\n")


if __name__ == "__main__":
    main()
