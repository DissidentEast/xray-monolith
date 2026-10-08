////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_vertex_base_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 19.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Graph vertex base class template inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _data_type,\
	typename _vertex_id_type,\
	typename _graph_type\
>

#define CSGraphVertex CVertex<\
	_data_type,\
	_vertex_id_type,\
	_graph_type\
>

TEMPLATE_SPECIALIZATION
IC CSGraphVertex::CVertex(const _data_type& data, const _vertex_id_type& vertex_id, size_t* edge_count)
{
	this->m_data = data;
	this->m_vertex_id = vertex_id;
	VERIFY(edge_count);
	this->m_edge_count = edge_count;
}

TEMPLATE_SPECIALIZATION
IC CSGraphVertex::~CVertex()
{
	while (!this->edges().empty())
		this->remove_edge(this->edges().back().vertex_id());

	while (!this->m_vertices.empty())
		this->m_vertices.back()->remove_edge(this->vertex_id());

	try
	{
		delete_data(this->m_data);
	}
	catch (...)
	{
	}
}

TEMPLATE_SPECIALIZATION
IC const typename CSGraphVertex::_edge_type*CSGraphVertex::edge(const _vertex_id_type& vertex_id) const
{
	typename CSGraphVertex::EDGES::const_iterator I = std::find(this->edges().begin(), this->edges().end(), vertex_id);
	if (this->m_edges.end() == I)
		return (0);
	return (&*I);
}

TEMPLATE_SPECIALIZATION
IC typename CSGraphVertex::_edge_type*CSGraphVertex::edge(const _vertex_id_type& vertex_id)
{
	typename CSGraphVertex::EDGES::iterator I = std::find(this->m_edges.begin(), this->m_edges.end(), vertex_id);
	if (this->m_edges.end() == I)
		return (0);
	return (&*I);
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::add_edge(CVertex* vertex, const _edge_weight_type& edge_weight)
{
	//	EDGES::iterator			I = std::find(m_edges.begin(),m_edges.end(),vertex->vertex_id());
	//	VERIFY					(m_edges.end() == I);
	vertex->on_edge_addition(this);
	this->m_edges.push_back(typename CSGraphVertex::_edge_type(edge_weight, vertex));
	++*this->m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::remove_edge(const _vertex_id_type& vertex_id)
{
	typename CSGraphVertex::EDGES::iterator I = std::find(this->m_edges.begin(), this->m_edges.end(), vertex_id);
	VERIFY(this->m_edges.end() != I);
	CVertex* vertex = (*I).vertex();
	vertex->on_edge_removal(this);
	this->m_edges.erase(I);
	--*this->m_edge_count;
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::on_edge_addition(CVertex* vertex)
{
	//	VERTICES::const_iterator	I = std::find(m_vertices.begin(),m_vertices.end(),vertex);
	//	VERIFY						(I == m_vertices.end());
	this->m_vertices.push_back(vertex);
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::on_edge_removal(const CVertex* vertex)
{
	typename CSGraphVertex::VERTICES::iterator I = std::find(this->m_vertices.begin(), this->m_vertices.end(), vertex);
	VERIFY(I != this->m_vertices.end());
	this->m_vertices.erase(I);
}

TEMPLATE_SPECIALIZATION
IC const _vertex_id_type&CSGraphVertex::vertex_id() const
{
	return (this->m_vertex_id);
}

TEMPLATE_SPECIALIZATION
IC const _data_type&CSGraphVertex::data() const
{
	return (this->m_data);
}

TEMPLATE_SPECIALIZATION
IC _data_type&CSGraphVertex::data()
{
	return (this->m_data);
}

TEMPLATE_SPECIALIZATION
IC void CSGraphVertex::data(const _data_type& data)
{
	this->m_data = data;
}

TEMPLATE_SPECIALIZATION
IC const typename CSGraphVertex::EDGES&CSGraphVertex::edges() const
{
	return (this->m_edges);
}

TEMPLATE_SPECIALIZATION
IC bool CSGraphVertex::operator==(const CVertex& obj) const
{
	if (this->vertex_id() != obj.vertex_id())
		return (false);

	if (!equal(this->edges(), obj.edges()))
		return (false);

	return (equal(this->data(), obj.data()));
}

#undef TEMPLATE_SPECIALIZATION
#undef CSGraphVertex
