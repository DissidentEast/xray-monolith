#ifndef pure_relcaseH
#define pure_relcaseH

// IGame_Level.h provides g_pGameLevel and CObjectList. It must be visible here:
// /permissive- checks this constructor template at its definition point, and the
// including TUs (via Feel_Touch.h/Feel_Vision.h) don't declare those names.
#include "IGame_Level.h"

class ENGINE_API pure_relcase
{
private:
	int m_ID;
public:
	template <typename class_type>
	pure_relcase(void (xr_stdcall class_type::* function_to_bind)(CObject*))
	{
		R_ASSERT(g_pGameLevel);
		// Downcast through void*: class_type reaches pure_relcase via Feel::Touch
		// with non-public inheritance, which /permissive- no longer accepts directly.
		class_type* self = static_cast<class_type*>(static_cast<void*>(this));
		g_pGameLevel->Objects.relcase_register(
			CObjectList::RELCASE_CALLBACK(
				self,
				function_to_bind
			),
			&m_ID
		);
	}

	virtual ~pure_relcase();
};

#endif // pure_relcaseH
