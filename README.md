# Newbxpotato

Newbxpotato is a lightweight Minecraft Bedrock RenderDragon shader based on
[Newb X MCPE](https://github.com/devendrn/newb-x-mcbe). It targets good mobile
performance without falling back to a flat vanilla look.

## Features

- Custom rounded cellular puffy clouds (same renderer as the main pack)
- Texture-based volumetric night aurora plus procedural aurora on the cloud layer
- Water surface waves and underwater wave effects
- Per-pixel cloud mirror on water, matching the sky cloud shapes 1:1
- Sky and aurora reflections on water
- Warm BSL-inspired lighting, fog and rain
- No alternate cloud subpacks: the custom cloud renderer is always the default
- Performance subpacks for disabling waves and/or fog

## Build

The GitHub Actions workflow builds a merged `.mcpack` (Android, Windows and iOS
in one file) on every push to `main`.
For a local build:

```bash
python -m pip install -r requirements.txt
python tool setup
python tool pack -p merged
```

Note: the merged profile includes HLSL (Direct3D) shaders, which can only be
compiled on Windows. On Linux/Android use `-p android` instead.

The finished pack is written to `build/`.

## Compatibility

- Android: MB Loader
- Windows: Wyvern Loader
- iOS: Minecraft with Hynis

## Credits

Based on Newb Shader / Newb X MCPE by devendrn. See `LICENSE` and the pack
copyright metadata for licensing details.
