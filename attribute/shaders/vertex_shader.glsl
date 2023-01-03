// #version 330 core

// // layout(std430, binding = 0) buffer TVertex
// // {
// //    vec4 vertex[]; 
// // };

// layout (std140) uniform BlockRect
// {
//     vec4 vertex[100];
// } u_Rect;


// uniform mat4  u_mvp;
// uniform vec2  u_resolution;
// uniform float u_thickness;

// void main()
// {
//     int line_i = gl_VertexID / 6;
//     int tri_i  = gl_VertexID % 6;

//     vec4 va[4];
//     for (int i=0; i<4; ++i)
//     {
//         va[i] = u_mvp * u_Rect.vertex[line_i+i];
//         va[i].xyz /= va[i].w;
//         va[i].xy = (va[i].xy + 1.0) * 0.5 * u_resolution;
//     }

//     vec2 v_line  = normalize(va[2].xy - va[1].xy);
//     vec2 nv_line = vec2(-v_line.y, v_line.x);

//     vec4 pos;
//     if (tri_i == 0 || tri_i == 1 || tri_i == 3)
//     {
//         vec2 v_pred  = normalize(va[1].xy - va[0].xy);
//         vec2 v_miter = normalize(nv_line + vec2(-v_pred.y, v_pred.x));

//         pos = va[1];
//         pos.xy += v_miter * u_thickness * (tri_i == 1 ? -0.5 : 0.5) / dot(v_miter, nv_line);
//     }
//     else
//     {
//         vec2 v_succ  = normalize(va[3].xy - va[2].xy);
//         vec2 v_miter = normalize(nv_line + vec2(-v_succ.y, v_succ.x));

//         pos = va[2];
//         pos.xy += v_miter * u_thickness * (tri_i == 5 ? 0.5 : -0.5) / dot(v_miter, nv_line);
//     }

//     pos.xy = pos.xy / u_resolution * 2.0 - 1.0;
//     pos.xyz *= pos.w;
//     gl_Position = pos;
// }

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float direction; 
layout(location = 2) in vec3 next;
layout(location = 3) in vec3 previous;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform float aspect;

uniform float thickness;
uniform int miter;

void main() {
  vec2 aspectVec = vec2(aspect, 1.0);
  mat4 projViewModel = projection * view * model;
  vec4 previousProjected = projViewModel * vec4(previous, 1.0);
  vec4 currentProjected = projViewModel * vec4(position, 1.0);
  vec4 nextProjected = projViewModel * vec4(next, 1.0);

  //get 2D screen space with W divide and aspect correction
  vec2 currentScreen = currentProjected.xy / currentProjected.w * aspectVec;
  vec2 previousScreen = previousProjected.xy / previousProjected.w * aspectVec;
  vec2 nextScreen = nextProjected.xy / nextProjected.w * aspectVec;

  float len = thickness;
  float orientation = direction;

  //starting point uses (next - current)
  vec2 dir = vec2(0.0);
  if (currentScreen == previousScreen) {
    dir = normalize(nextScreen - currentScreen);
  } 
  //ending point uses (current - previous)
  else if (currentScreen == nextScreen) {
    dir = normalize(currentScreen - previousScreen);
  }
  //somewhere in middle, needs a join
  else {
    //get directions from (C - B) and (B - A)
    vec2 dirA = normalize((currentScreen - previousScreen));
    if (miter == 1) {
      vec2 dirB = normalize((nextScreen - currentScreen));
      //now compute the miter join normal and length
      vec2 tangent = normalize(dirA + dirB);
      vec2 perp = vec2(-dirA.y, dirA.x);
      vec2 miter = vec2(-tangent.y, tangent.x);
      dir = tangent;
      len = thickness / dot(miter, perp);
    } else {
      dir = dirA;
    }
  }
  vec2 normal = vec2(-dir.y, dir.x);
  normal *= len/2.0;
  normal.x /= aspect;

  vec4 offset = vec4(normal * orientation, 0.0, 1.0);
  gl_Position = currentProjected + offset;
  gl_PointSize = 1.0;
}