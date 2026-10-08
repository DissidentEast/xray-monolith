////////////////////////////////////////////////////////////////////////////
//	Module 		: path_manager_level_straight_line_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Level straight line path manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION \
	template <\
		typename _DataStorage,\
		typename _dist_type,\
		typename _index_type,\
		typename _iteration_type\
	>

#define CLevelStraightLinePathManager CPathManager<\
	CLevelGraph,\
	_DataStorage,\
	SStraightLineParams<\
		_dist_type,\
		_index_type,\
		_iteration_type\
	>,\
	_dist_type,\
	_index_type,\
	_iteration_type\
>


TEMPLATE_SPECIALIZATION
CLevelStraightLinePathManager::~CPathManager()
{
}

TEMPLATE_SPECIALIZATION
IC void CLevelStraightLinePathManager::setup(
	const _Graph* _graph,
	_DataStorage* _data_storage,
	xr_vector<_index_type>* _path,
	const _index_type& _start_node_index,
	const _index_type& _goal_node_index,
	_Parameters& parameters
)
{
	inherited::setup(
		_graph,
		_data_storage,
		_path,
		_start_node_index,
		_goal_node_index,
		parameters
	);
	this->m_parameters = &parameters;
	this->m_parameters->m_distance = this->m_parameters->max_range;
}

TEMPLATE_SPECIALIZATION
template <typename T>
IC void CLevelStraightLinePathManager::create_path(T& vertex)
{
	inherited::create_path(vertex);

	_dist_type fCumulativeDistance = 0, fLastDirectDistance = 0, fDirectDistance;

	Fvector tPosition = this->m_parameters->m_start_point;

	typename xr_vector<_index_type>::iterator I = this->path->begin();
	typename xr_vector<_index_type>::iterator E = this->path->end();
	_index_type& dwNode = *I;
	for (++I; I != E; ++I)
	{
		u32 vertex_id = this->graph->check_position_in_direction(dwNode, tPosition, this->graph->vertex_position(*I));
		if (this->graph->valid_vertex_id(vertex_id))
			fDirectDistance = tPosition.distance_to(this->graph->vertex_position(*I));
		else
			fDirectDistance = this->m_parameters->max_range;
		if (fDirectDistance == this->m_parameters->max_range)
		{
			if (fLastDirectDistance == 0)
			{
				fCumulativeDistance += this->graph->distance(dwNode, *I);
				dwNode = *I;
			}
			else
			{
				fCumulativeDistance += fLastDirectDistance;
				fLastDirectDistance = 0;
				dwNode = *--I;
			}
			tPosition = this->graph->vertex_position(dwNode);
		}
		else
			fLastDirectDistance = fDirectDistance;
		if (fCumulativeDistance + fLastDirectDistance >= this->m_parameters->max_range)
		{
			this->m_parameters->m_distance = this->m_parameters->max_range;
			return;
		}
	}

	u32 vertex_id = this->graph->check_position_in_direction(dwNode, tPosition, this->m_parameters->m_dest_point);
	if (this->graph->valid_vertex_id(vertex_id))
		fDirectDistance = tPosition.distance_to(this->m_parameters->m_dest_point);
	else
		fDirectDistance = this->m_parameters->max_range;
	if (fDirectDistance == this->m_parameters->max_range)
		this->m_parameters->m_distance = fCumulativeDistance + fLastDirectDistance + this->m_parameters
		                                                                       ->m_dest_point.distance_to(
			                                                                       this->graph->vertex_position(
				                                                                       (*this->path)[this->path->size() - 1]));
	else
		this->m_parameters->m_distance = fCumulativeDistance + fDirectDistance;
}

#undef TEMPLATE_SPECIALIZATION
#undef CLevelStraightLinePathManager
