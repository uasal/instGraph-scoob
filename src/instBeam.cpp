#include "instBeam.hpp"
#include "instGraph.hpp"

#include <iostream>

namespace ingr
{

instBeam::instBeam()
{
}

instBeam::instBeam( const instBeam &ib )
{
    m_name = ib.m_name;
    m_source = ib.m_source;
    m_dest = ib.m_dest;
    m_state = ib.m_state;
}

std::string instBeam::name()
{
    return m_name;
}

void instBeam::name( const std::string &n )
{
    m_name = n;
}

bool instBeam::sourceValid() const
{
    return ( m_source != nullptr );
}

instIOPut *instBeam::source()
{
    if( m_source == nullptr )
    {
        throw std::out_of_range( "instBeam::source() attempt to access m_source which is null" );
    }

    return m_source;
}

void instBeam::source( instIOPut *inp )
{
    m_source = inp;
}

bool instBeam::destValid() const
{
    return ( m_dest != nullptr );
}

instIOPut *instBeam::dest()
{
    if( m_dest == nullptr )
    {
        throw std::out_of_range( "instBeam::dest() attempt to access m_dest which is null" );
    }

    return m_dest;
}

void instBeam::dest( instIOPut *outp )
{
    m_dest = outp;
}

beamState instBeam::state()
{
    return m_state;
}

void instBeam::parentGraph( instGraph *ig )
{
    m_parentGraph = ig;
}

bool instBeam::auxDataValid()
{
    return ( m_auxData != nullptr );
}

void *instBeam::auxData()
{
    if( m_auxData == nullptr )
    {
        throw std::out_of_range( "instBeam::auxData(): attemmpt to accesss m_auxData pointer which is null" );
    }

    return m_auxData;
}

void instBeam::auxData( void *ad )
{
    m_auxData = ad;
}

std::string instBeam::key()
{
    return m_name;
}

void instBeam::stateChange()
{
    // First handle cases where source or dest are null pointers

    // if m_source is null, then nothing else matters
    if( m_source == nullptr )
    {
        if( m_state == beamState::off )
            return; // not a state change

        m_state = beamState::off;

        if( m_dest )
        {
            if( m_dest->state() == putState::on )
            {
                m_dest->state( putState::waiting, true );
            }
        }

        if( m_parentGraph )
            m_parentGraph->stateChange();

        return;
    }

    // if m_dest is null, the beam can only be intermediate or off
    if( m_dest == nullptr )
    {
        if( m_source->state() == putState::on )
        {
            m_state = beamState::intermediate;
        }
        else if( m_state == beamState::off )
        {
            return; // no state change
        }
        else
        {
            m_state = beamState::off;
        }

        if( m_parentGraph )
            m_parentGraph->stateChange();

        return;
    }

    // No nullptrs
    if( m_source->state() == putState::waiting )
    {
        if( m_state == beamState::off )
            return;
        m_state = beamState::off;

        if( m_dest->state() == putState::on )
        {
            m_dest->state( putState::waiting, true );
        }

        if( m_parentGraph )
            m_parentGraph->stateChange();

        return;
    }
    else if( m_source->state() == putState::on )
    {
        if( m_dest->state() == putState::on || m_dest->state() == putState::waiting )
        {
            if( m_state == beamState::on )
            {
                return;
            }
            m_state = beamState::on;

            m_dest->state( putState::on, true );
        }
        else
        {
            if( m_state == beamState::intermediate )
            {
                return;
            }
            m_state = beamState::intermediate;
        }

        if( m_parentGraph )
            m_parentGraph->stateChange();

        return;
    }
    else
    {
        if( m_state == beamState::off )
        {
            return;
        }

        m_state = beamState::off;

        if( m_dest->state() == putState::on )
        {
            m_dest->state( putState::waiting, true );
        }

        if( m_parentGraph )
            m_parentGraph->stateChange();
        return;
    }
}

} // namespace ingr
