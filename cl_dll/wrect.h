//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined( WRECTH )
#define WRECTH

/*
typedef struct rect_s
{
	int				left, right, top, bottom;
} wrect_t;
*/
;
class wrect_t
{
public:
	int	left, right, top, bottom;
	wrect_t()
	{}
	wrect_t(wrect_t& copy)
	{
		left = copy.left;
		bottom = copy.bottom;
		top = copy.top;
		right = copy.right;
	}
};

#endif