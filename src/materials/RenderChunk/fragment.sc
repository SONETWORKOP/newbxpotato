$input v_color0, v_color1, v_fog, v_refl, v_texcoord0, v_lightmapUV, v_extra, v_position, v_reflPbr, v_reflSun

#include <bgfx_shader.sh>
#include <newb/main.sh>

SAMPLER2D_AUTOREG(s_MatTexture);
SAMPLER2D_AUTOREG(s_SeasonsTexture);
SAMPLER2D_AUTOREG(s_LightMapTexture);

uniform vec4 CameraPosition;
uniform vec4 ViewPositionAndTime;
uniform vec4 FogColor;

// (Water cloud-mirror HATAYA - paani me clouds reflection nahi.
// Sirf procedural aurora aks rahega neeche.)

void main() {
  #if defined(DEPTH_ONLY_OPAQUE) || defined(DEPTH_ONLY) || defined(INSTANCING)
    gl_FragColor = vec4(1.0,1.0,1.0,1.0);
    return;
  #endif

  vec4 diffuse = texture2D(s_MatTexture, v_texcoord0);
  vec4 color = v_color0;

  #ifdef ALPHA_TEST
    if (diffuse.a < 0.6) {
      discard;
    }
  #endif

  #if defined(SEASONS) && (defined(OPAQUE) || defined(ALPHA_TEST))
    diffuse.rgb *= mix(vec3(1.0,1.0,1.0), texture2D(s_SeasonsTexture, v_color1.xy).rgb * 2.0, v_color1.z);
  #endif

  vec3 glow = nlGlow(s_MatTexture, v_texcoord0, v_extra.a);

  diffuse.rgb *= diffuse.rgb;

  #if defined(TRANSPARENT) && !(defined(SEASONS) || defined(RENDER_AS_BILLBOARDS))
    if (v_extra.b > 0.9) {
      diffuse.rgb = vec3_splat(1.0 - NL_WATER_TEX_OPACITY*(1.0 - diffuse.b*1.8));
      diffuse.a = color.a;
    }
  #else
    diffuse.a = 1.0;
  #endif

  diffuse.rgb *= color.rgb;
  diffuse.rgb += glow;

  if (v_extra.b > 0.9) {
    diffuse.rgb += v_refl.rgb*v_refl.a;

    // aurora-only mirror (clouds hataye) - raat me paani par purple glow.
    // Procedural renderAurora (bina texture), sky wale purple rang me.
    #ifdef NL_AURORA
      {
        vec3 aurV = normalize(v_reflPbr.xyz);
        vec3 aurReflDir = vec3(-aurV.x, aurV.y, -aurV.z);
        if (aurReflDir.y > 0.004) {
          float aurVdotU = clamp(aurReflDir.y, 0.0, 1.0);
          float aurNight = 1.0 - smoothstep(-0.02, 0.32, v_reflSun.w);
          if (aurNight > 0.001 && aurVdotU > 0.15) {
            vec3 aurP = aurReflDir;
            aurP.xz /= max(0.0001, aurP.y);
            vec4 aur = renderAurora(aurP.xyy, ViewPositionAndTime.w, v_reflPbr.w, FogColor.rgb);
            #ifdef NL_WATER_AURORA_MIRROR
              float auroraAmt = NL_WATER_AURORA_MIRROR;
            #else
              float auroraAmt = 0.55;
            #endif
            diffuse.rgb += aur.rgb*aur.a*aurNight*auroraAmt;
          }
        }
      }
    #endif
  } else if (v_refl.a > 0.0) {
    // reflective effect - only on xz plane (ground / flat smooth blocks)
    float dy = abs(dFdy(v_extra.g));
    if (dy < 0.0002) {
      float mask = v_refl.a*(clamp(v_extra.r*10.0,8.2,8.8)-7.8);

      #ifdef NL_PBR_BLOCK_REFL
        // === V3 PBR block reflection (normal-map + TBN + Cook-Torrance) ===
        vec3 viewDir = v_reflPbr.xyz;
        float rainFactor = v_reflPbr.w;
        vec3 sunDir = v_reflSun.xyz;

        // normal from the block's own texture luminance gradient (V3 getNormal)
        // texel size approx for a 16px block tile in the atlas
        vec3 texNormal = nlTexNormal(s_MatTexture, v_texcoord0, NL_PBR_NORMAL_TEXEL, NL_PBR_NORMAL_STRENGTH);

        // rain makes the ground wetter -> stronger, smoother mirror
        float pbrMask = mask;
        #ifdef NL_PBR_RAIN_BOOST
          pbrMask *= 1.0 + NL_PBR_RAIN_BOOST*rainFactor;
        #endif
        pbrMask = min(pbrMask, 1.0);

        // drop some diffuse so the mirror reads through, then apply V3 layer
        diffuse.rgb *= 1.0 - 0.6*pbrMask;
        diffuse.rgb = nlApplyPbrRefl(diffuse.rgb, v_refl.rgb, pbrMask, texNormal, v_position, viewDir, sunDir);
      #else
        // RTX-style: drop diffuse and push the mirror reflection forward so
        // smooth blocks (iron, diamond, quartz) read like a true mirror
        diffuse.rgb *= 1.0 - 0.75*mask;
        diffuse.rgb += v_refl.rgb*mask*1.15;
      #endif
    }
  }

  diffuse.rgb = mix(diffuse.rgb, v_fog.rgb, v_fog.a);

  diffuse.rgb = colorCorrection(diffuse.rgb);

  gl_FragColor = diffuse;
}
