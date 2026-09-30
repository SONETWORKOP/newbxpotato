#ifndef NL_CONFIG_H
#define NL_CONFIG_H

/*
  Newbxpotato - lightweight Newb X MCPE edition
  Based on Newb Shader (https://github.com/devendrn/newb-x-mcbe)
  Custom puffy clouds, aurora, water waves and balanced mobile rendering.
*/

/* Color correction - clean and neutral, not a filmic/BSL grade.
   Newbxpotato aims for readable vanilla-plus color: highlights roll off but
   midtones stay where vanilla put them, so terrain textures keep their detail. */
#define NL_TONEMAP_TYPE 3              // Extended Reinhard - natural highlight rolloff
#define NL_GAMMA 1.14                  // mild contrast, keeps shadow detail readable
#define NL_EXPOSURE 1.05               // near-neutral; brightness comes from sunlight
#define NL_SATURATION 1.18             // gentle lift - colorful without going neon
//#define NL_TINT                      // OFF
#define NL_TINT_LOW  vec3(0.3,0.5,1.4)
#define NL_TINT_HIGH vec3(1.4,0.7,0.3)

/* Lighting - moderate directional light with soft ambient fill.
   Lower sunlight + higher ambient than a BSL-style grade: shadows stay
   visible instead of crushing to black on a phone screen in daylight. */
#define NL_SUNLIGHT_INTENSITY   3.6    // moderate direct sun
#define NL_TORCHLIGHT_INTENSITY 1.35   // warm but not blown-out torches
#define NL_SHADOW_INTENSITY     1.25   // softer shadows, keeps detail in the dark
#define NL_MIN_LIGHTING_BOOST   1.12   // a little ambient fill so caves stay readable
//#define NL_BLINKING_TORCH
//#define NL_CLOUD_SHADOW              // OFF: expensive noise on chunk vertices

/* Ambient */
#define NL_NETHER_AMBIENT vec3(2.7,1.95,1.7)
#define NL_END_AMBIENT    vec3(1.75,1.15,2.15)

/* Sun/moon - warm day, purple-leaning night to match the aurora */
#define NL_DAWN_SUNLIGHT_COL   vec3(1.14,0.60,0.26)   // soft amber sunrise
#define NL_NOON_SUNLIGHT_COL   vec3(1.05,1.01,0.92)   // near-neutral daylight
#define NL_NIGHT_MOONLIGHT_COL vec3(0.06,0.07,0.24)   // violet-tinted moonlight

/* Torch */
#define NL_OVERWORLD_TORCH_COL  vec3(1.0,0.58,0.24)
#define NL_UNDERWATER_TORCH_COL vec3(1.0,0.6,0.3)
#define NL_NETHER_TORCH_COL     vec3(1.0,0.52,0.2)
#define NL_END_TORCH_COL        vec3(0.96,0.58,0.34)

/* Fog - light touch so render distance stays visible on mobile */
#define NL_FOG 1.12
#define NL_MIST_DENSITY 0.3
#define NL_RAIN_MIST_OPACITY 0.36
//#define NL_CLOUDY_FOG                // OFF: keep distance fog, skip animated fog noise

/* Height fog */
#define NL_HEIGHT_FOG 0.42
#define NL_HEIGHT_FOG_START 58.0
#define NL_HEIGHT_FOG_RANGE 50.0

/* Sky */
#define NL_SKY_VOID_FACTOR     0.5
#define NL_SKY_VOID_DARKNESS   0.3
#define NL_SKY_RAIN_MIX_FACTOR 0.95

/* Sky colors - cool daylight blue, violet night that carries the aurora */
#define NL_DAWN_ZENITH_COL   vec3(0.22,0.26,0.70)     // dusky indigo overhead
#define NL_DAWN_HORIZON_COL  vec3(2.60,0.78,0.34)     // amber sunrise, tamed red
#define NL_DAWN_EDGE_COL     vec3(2.85,1.32,0.62)     // soft golden rim
#define NL_DAY_ZENITH_COL    vec3(0.16,0.44,1.75)     // clear sky blue
#define NL_DAY_HORIZON_COL   vec3(0.62,1.02,1.52)     // pale haze at the horizon
#define NL_DAY_EDGE_COL      vec3(1.10,1.32,1.55)     // bright atmospheric edge
#define NL_NIGHT_ZENITH_COL  vec3(0.016,0.032,0.105)   // deep navy blue upar (gallery image)
#define NL_NIGHT_HORIZON_COL vec3(0.070,0.120,0.240)   // steel-blue horizon glow
#define NL_NIGHT_EDGE_COL    vec3(0.085,0.140,0.260)   // soft blue rim
#define NL_RAIN_ZENITH_COL   vec3(0.30,0.33,0.39)     // overcast slate
#define NL_RAIN_HORIZON_COL  vec3(0.44,0.47,0.51)
#define NL_END_ZENITH_COL    vec3(0.07,0.004,0.11)
#define NL_END_HORIZON_COL   vec3(0.55,0.03,0.58)

/* Rainbow */
#define NL_RAINBOW
#define NL_RAINBOW_CLEAR 0.0
#define NL_RAINBOW_RAIN  0.45

/* Ore glow */
#define NL_GLOW_TEX 2.4
//#define NL_GLOW_SHIMMER              // OFF: emissive textures still glow
#define NL_GLOW_SHIMMER_SPEED 0.9
#define NL_GLOW_LEAK 0.42

/* Waving */
#define NL_PLANTS_WAVE 0.048
#define NL_LANTERN_WAVE 0.13
#define NL_WAVE_SPEED 2.4
#define NL_WAVE_RANGE 12.0

/* Water - mirror-leaning surface so the cloud reflection stays the highlight */
#define NL_WATER_TRANSPARENCY 0.88      // a bit of body so reflections read clearly
#define NL_WATER_BUMP 0.19              // gentle ripples - hard ripples shred the mirror
#define NL_WATER_WAVE_SPEED  0.45       // slow swell
#define NL_WATER_TEX_OPACITY 0.12       // mostly reflection, minimal water texture
#define NL_WATER_CLOUD_MIRROR 0.42      // per-pixel cloud mirror on water (1.0 = full)
#define NL_WATER_CLOUD_HEIGHT 192.0     // cloud height used by plane cloud samplers
#define NL_WATER_CLOUD_REFLECTION_DEPTH 2.0 // clouds appear this many blocks below the surface
#define NL_WATER_CLOUD_REFL_RIPPLE 0.012 // very subtle swell drift (0.0 = dead-flat mirror)
#define NL_WATER_AURORA_MIRROR 0.55     // paani me aurora aks (clouds nahi, sirf purple glow)
#define NL_WATER_WAVE
//#define NL_WATER_REFL_MASK             // OFF: full reflection instead of patchy masked reflection
#define NL_WATER_TINT vec3(0.22,0.58,0.80) // cool teal-blue

/* Underwater */
#define NL_UNDERWATER_BRIGHTNESS 0.95
#define NL_CAUSTIC_INTENSITY 1.8
#define NL_UNDERWATER_WAVE 0.09
#define NL_UNDERWATER_STREAKS 0.9
#define NL_UNDERWATER_TINT vec3(0.72,0.9,1.0)

/* Custom cloud renderer - always default, no cloud subpack required */
#define NL_CLOUD_TYPE 4            // 4=pixelated layered clouds (default)

/* Pixel clouds (NL_CLOUD_TYPE 4) - also mirrored on water.
   STEPS is the main performance knob: each step is one hash + one step(). */
#define NL_PIXEL_CLOUD_STEPS 10       // lattice layers (10 = reference look)
#define NL_PIXEL_CLOUD_SCALE 5.0      // lattice density on the cloud plane
#define NL_PIXEL_CLOUD_SPEED 0.06     // drift speed (cells/sec)
#define NL_PIXEL_CLOUD_COVERAGE 0.7   // hash threshold: higher = fewer clouds
#define NL_PIXEL_CLOUD_WORLD_SCALE 0.0016 // world-lock: clouds stay put as you walk
#define NL_PIXEL_CLOUD_OPACITY 0.88   // max cloud opacity
#define NL_PIXEL_CLOUD_SHADING 0.62   // strength of the internal shaded layer
#define NL_PIXEL_CLOUD_DAY_COL vec3(1.06,1.04,1.00)   // sunlit cloud tops
#define NL_PIXEL_CLOUD_NIGHT_COL vec3(0.19,0.22,0.34) // night cloud tops

/* Sky-dome procedural clouds - extra cloud layer drawn on the sky itself.
   OFF by default: type 4 clouds already fill the sky and the dome pass costs
   an extra lattice lookup on every sky pixel. */
//#define NL_SKY_CLOUDS
#define NL_SKY_CLOUD_SPEED 0.09    // drift speed (cell units/sec)
#define NL_SKY_CLOUD_DIR vec2(1.0, 0.35) // wind direction the dome clouds drift along
#define NL_SKY_CLOUD_OPACITY 0.9   // max cloud opacity

/* Soft cloud */
#define NL_CLOUD1_SCALE vec2(0.011, 0.015)
#define NL_CLOUD1_DEPTH 3.4
#define NL_CLOUD1_SPEED 0.028       // slow drift saves animation work
#define NL_CLOUD1_DENSITY 0.52
#define NL_CLOUD1_OPACITY 0.92

/* Vanilla cloud */
#define NL_CLOUD0_THICKNESS 2.1
#define NL_CLOUD0_RAIN_THICKNESS 4.0
#define NL_CLOUD0_OPACITY 0.9
#define NL_CLOUD0_MULTILAYER
/* Rounded cloud */
#define NL_CLOUD2_THICKNESS 2.1
#define NL_CLOUD2_RAIN_THICKNESS 2.5
#define NL_CLOUD2_STEPS 5
#define NL_CLOUD2_SCALE vec2(0.033, 0.033)
#define NL_CLOUD2_SHAPE vec2(0.5, 0.4)
#define NL_CLOUD2_DENSITY 25.0
#define NL_CLOUD2_VELOCITY 0.8
#define NL_CLOUD2_LAYER2_OFFSET 143.0
#define NL_CLOUD2_LAYER2_THICKNESS 2.5
#define NL_CLOUD2_LAYER2_RAIN_THICKNESS 3.0
#define NL_CLOUD2_LAYER2_STEPS 3
#define NL_CLOUD2_LAYER2_SCALE vec2(0.03, 0.03)
#define NL_CLOUD2_LAYER2_SHAPE vec2(0.5, 0.4)
#define NL_CLOUD2_LAYER2_DENSITY 25.0
#define NL_CLOUD2_LAYER2_VELOCITY 0.8

/* Realistic cloud */
#define NL_CLOUD3_SCALE vec2(0.03, 0.03)
#define NL_CLOUD3_SPEED 0.005
#define NL_CLOUD3_SHADOW 0.9
#define NL_CLOUD3_SHADOW_OFFSET 0.3

/* Aurora - night sky aurora borealis (purple curtain, noise-texture driven) */
#define NL_AURORA 0.85             // master aurora toggle/strength
#define NL_AURORA_LAYERS 10        // curtain layers (reference 20) - main cost knob
#define NL_AURORA_BRIGHTNESS 1.15  // overall curtain brightness
#define NL_AURORA_COL_LOW vec3(7.0,1.6,13.0)  // deep violet, near layers
#define NL_AURORA_COL_HIGH vec3(3.4,1.0,9.0)  // magenta-purple, far layers
#define NL_AURORA_VELOCITY 0.03
#define NL_AURORA_SCALE 0.04
#define NL_AURORA_WIDTH 0.18
#define NL_AURORA_COL1 vec3(0.42,0.16,0.78)  // reflected aurora, lower band
#define NL_AURORA_COL2 vec3(0.66,0.30,1.00)  // reflected aurora, upper band
#define NL_CLOUD_AURORA_REFLECTION

/* Shooting star */
//#define NL_SHOOTING_STAR             // OFF: avoids full-sky animated streak math
#define NL_SHOOTING_STAR_PERIOD 8.0
#define NL_SHOOTING_STAR_DELAY 24.0

/* Galaxy - animated stars at night */
//#define NL_GALAXY_STARS              // OFF: vanilla stars remain, skip 3D noise
#define NL_GALAXY_VIBRANCE 0.25
#define NL_GALAXY_SPEED 0.02
#define NL_GALAXY_DAY_VISIBILITY 0.0

/* Sun/Moon */
#define NL_SUN_SIZE  1.1
#define NL_MOON_SIZE 1.0
#define NL_SUN_PATH_YAW    15.0
#define NL_MOON_PATH_YAW   17.0
#define NL_SUN_PATH_TILT   31.0
#define NL_MOON_PATH_TILT -28.0
#define NL_SUN_TILT        45.0
#define NL_MOON_TILT       45.0

/* Godrays - subtle shafts, kept cheap */
#define NL_GODRAY 0.9

/* Ground reflection - RTX-style mirror reflection on smooth blocks */
//#define NL_GROUND_REFL                // OFF: water/cloud reflection remains enabled
#define NL_GROUND_RAIN_WETNESS 1.3     // wet sheen while raining
#define NL_GROUND_RAIN_PUDDLES 0.75    // puddle coverage

/* PBR block reflection (from "block reflection V3") - fragment-stage
   normal-mapped, TBN-distorted, Cook-Torrance mirror on smooth blocks */
//#define NL_PBR_BLOCK_REFL             // OFF: avoids four texture taps + GGX per pixel
#define NL_PBR_ROUGHNESS 0.45          // higher = softer glint (low values cause speckles)
#define NL_PBR_METALLIC 0.0            // 0 keeps texture bright (V3 note)
#define NL_PBR_F0 vec3(0.10,0.10,0.11) // subtle reflectance (V3 iron 0.56 was too hot)
#define NL_PBR_SUNCOLOR vec3(1.0,0.95,0.85) // neutral white glint, not golden
#define NL_PBR_SPEC_INTENSITY 0.25     // overall sun glint strength
#define NL_PBR_SPEC_CLAMP 0.6          // caps GGX spikes -> kills golden fireflies
#define NL_PBR_NORMAL_TEXEL 0.0025     // texel size for normal-map gradient
#define NL_PBR_NORMAL_STRENGTH 0.7     // bump strength (2.0 was too noisy)
#define NL_PBR_RAIN_BOOST 1.4          // extra mirror strength when raining (wet ground)

/* Rain reflection - strong wet/puddle mirror only while raining */
//#define NL_RAIN_REFL_STRENGTH         // OFF: use lightweight built-in rain wetness

/* Entity */
#define NL_ENTITY_BRIGHTNESS     0.62
#define NL_ENTITY_EDGE_HIGHLIGHT 0.34

/* Weather - thin, wind-driven rain */
#define NL_WEATHER_SPECK 0.42            // matte speck, no plastic highlight
#define NL_WEATHER_RAIN_SLANT 4.2        // wind slant
#define NL_WEATHER_PARTICLE_SIZE 0.78    // thin raindrops

/* Lava */
//#define NL_LAVA_NOISE                 // OFF: animated lava texture remains visible
#define NL_LAVA_NOISE_SPEED 0.2

/* ---- SUBPACK CONFIG ---- */
#ifdef LITE
  #define NO_WAVE
  #undef NL_GLOW_SHIMMER
  #undef NL_LAVA_NOISE
  #undef NL_WEATHER_SPECK
  #undef NL_SHOOTING_STAR
  #undef NL_CLOUD_AURORA_REFLECTION
  #undef NL_UNDERWATER_STREAKS
  #undef NL_RAIN_MIST_OPACITY
  #undef NL_CLOUDY_FOG
  #undef NL_ENTITY_EDGE_HIGHLIGHT
  #undef NL_PBR_BLOCK_REFL
  // halve the two per-pixel loops: aurora curtain and cloud lattice
  #undef NL_AURORA_LAYERS
  #define NL_AURORA_LAYERS 5
  #undef NL_PIXEL_CLOUD_STEPS
  #define NL_PIXEL_CLOUD_STEPS 6
#endif

#ifdef NO_WAVE_NO_FOG
  #define NO_WAVE
  #define NO_FOG
  #define NL_NO_WATER_CLOUD_REFL
#endif

#ifdef NO_FOG
  #undef NL_FOG
  #define NL_NO_WATER_CLOUD_REFL
#endif

#ifdef NO_WAVE
  #undef NL_PLANTS_WAVE
  #undef NL_LANTERN_WAVE
  #undef NL_UNDERWATER_WAVE
  #undef NL_WATER_WAVE
  #undef NL_RAIN_MIST_OPACITY
  #define NL_NO_WATER_CLOUD_REFL
#endif

#ifdef CHUNK_ANIM
  #define NL_CHUNK_LOAD_ANIM 100.0
#endif

#endif
