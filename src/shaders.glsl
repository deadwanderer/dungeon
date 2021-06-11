@ctype mat4 glm::mat4
@ctype vec2 glm::vec2
@ctype vec3 glm::vec3
@ctype vec4 glm::vec4

@vs surfaceVS
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;

uniform surface_vs_params {
  mat4 viewproj;
  mat4 model;
  vec3 color;
};

out vec2 out_texcoord;
out vec3 out_color;

void main() {
  gl_Position = viewproj * model * vec4(position, 1.0);
  out_texcoord = texcoord;
  out_color = color;
}
@end

@fs surfaceFS
in vec2 out_texcoord;
in vec3 out_color;

out vec4 frag_color;

void main() {
  frag_color = vec4(out_color, 1.0);
}
@end

@program surface surfaceVS surfaceFS