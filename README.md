# Newbxpotato

Newbxpotato is a lightweight Minecraft Bedrock RenderDragon shader based on
[Newb X MCPE](https://github.com/devendrn/newb-x-mcbe). It targets good mobile
performance without falling back to a flat vanilla look.

## Features

- Custom puffy clouds adapted from the supplied `cloud.txt` design
- Green/cyan aurora adapted to Newb's lightweight procedural pipeline
- Water surface waves and underwater wave effects
- Sky, cloud and aurora reflections on water
- Warm BSL-inspired lighting, fog and rain
- No alternate cloud subpacks: the custom cloud renderer is always the default
- Performance subpacks for disabling waves and/or fog

## Build

The GitHub Actions workflow builds an Android/MB Loader `.mcpack` on every push
to `main`.
For a local build:

```bash
python -m pip install -r requirements.txt
python tool setup
python tool pack -p android
```

The finished pack is written to `build/`.

## Compatibility

- Android: MB Loader
- Windows: Wyvern Loader
- iOS: Minecraft with Hynis

## Credits

Based on Newb Shader / Newb X MCPE by devendrn. See `LICENSE` and the pack
copyright metadata for licensing details.
