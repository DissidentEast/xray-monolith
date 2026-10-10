#pragma once

#include "state_defs.h"
#include "control_com_defs.h"
#include "ai_monster_defs.h"
// monster_squad(), ai() and CLevelGraph must be visible at the template
// definition points in the state *_inline.h headers; /permissive- checks them
// before the including TU's later includes become visible.
#include "ai_monster_squad_manager.h"
#include "../../ai_space.h"
#include "../../level_graph.h"

// Shared time helper, historically defined in monster_state_attack_on_run.h and
// visible to other state headers only by include order luck. It must be visible
// at every template definition point, so it lives here.
inline TTime current_time() { return Device.dwTimeGlobal; }

// Lain: added
#ifdef DEBUG
#include "debug_text_tree.h"
#endif

template <typename _Object>
class CState
{
protected:
	typedef CState<_Object> CSState;
public:
	CState(_Object* obj, void* data = 0);
	virtual ~CState();

	virtual void reinit();
	virtual void remove_links(CObject* object) = 0;

	virtual void initialize();
	virtual void execute();
	virtual void finalize();
	virtual void critical_finalize();

	virtual void reset();

	virtual bool check_completion() { return false; }
	virtual bool check_start_conditions() { return true; }

	virtual void reselect_state()
	{
	}

	virtual void check_force_state()
	{
	}

	CSState* get_state(u32 state_id);
	CSState* get_state_current();

	void fill_data_with(void* ptr_src, u32 size);

	u32 time_started() { return time_state_started; }

	virtual bool check_control_start_conditions(ControlCom::EControlType type);

	// Lain: added
#ifdef DEBUG
	virtual void		add_debug_info          (debug::text_tree& root_s);
#endif

protected:
	void select_state(u32 new_state_id);
	void add_state(u32 state_id, CSState* s);

	virtual void setup_substates()
	{
	}

	EMonsterState get_state_type();

	u32 current_substate;
	u32 prev_substate;

	u32 time_state_started;

	_Object* object;

	void* _data;

private:
	void free_mem();

	typedef xr_map<u32, CSState*> SubStates;
	SubStates substates;
	typedef typename xr_map<u32, CSState*>::iterator STATE_MAP_IT;
};

template <typename _Object>
class CStateMove : public CState<_Object>
{
protected:
	typedef CState<_Object> inherited;
public:
	CStateMove(_Object* obj, void* data = 0) : inherited(obj, data)
	{
	}

	virtual ~CStateMove()
	{
	}

	virtual void initialize()
	{
		inherited::initialize();
		this->object->path().prepare_builder();
	}
};


#include "state_inline.h"
