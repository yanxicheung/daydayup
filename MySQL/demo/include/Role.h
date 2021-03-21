#ifndef H05B2224D_B926_4FC0_A936_87B52B8A98DB
#define H05B2224D_B926_4FC0_A936_87B52B8A98DB

#define ROLE(role)  get##role()
#define ROLE_PROTO_TYPE(role) role& ROLE(role) const

#define IMPL_ROLE(role)                                       \
ROLE_PROTO_TYPE(role)                                         \
{                                                             \
    return const_cast<role&>(static_cast<const role&>(*this));\
}

#endif
