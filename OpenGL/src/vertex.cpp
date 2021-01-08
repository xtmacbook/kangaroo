
#include "vertex.h"

Vertex_P::Vertex_P() {}
Vertex_P::Vertex_P(const V3f&p) :position_(p) {}

Vertex_PT::Vertex_PT(){}
Vertex_PT::Vertex_PT(const V3f&p, const V2f&t) : Vertex_P(p), texCoords_(t) {}

Vertex_PC::Vertex_PC() {}
Vertex_PC::Vertex_PC(const V3f&p, const V3f&c) : Vertex_P(p), color_(c) {}

Vertex_PN::Vertex_PN() {}
Vertex_PN::Vertex_PN(const V3f&p, const V3f&n) : Vertex_P(p), normal_(n) {}

Vertex_PTBN::Vertex_PTBN() {}
Vertex_PTBN::Vertex_PTBN(const V3f&p, const V3f&t, const V3f&b) : Vertex_P(p), tangent_(t), bitangent_(b) {}

Vertex_PNT::Vertex_PNT(){}
Vertex_PNT::Vertex_PNT(const V3f&p, const V3f&n, const V2f&t):Vertex_PN(p,n),texCoords_(t){}

Vertex_PNTTBN::Vertex_PNTTBN() {}
Vertex_PNTTBN::Vertex_PNTTBN(const V3f&p, const V3f&n, const V2f&t, const V3f& tangent, const V3f&bitangent) : Vertex_PNT(p, n, t), tangent_(tangent), bitangent_(bitangent) {}
