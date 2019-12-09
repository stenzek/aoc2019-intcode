#include "scope_timer.h"
#include <cstdio>

ScopeTimer::ScopeTimer(std::string_view scope_name) : m_scope_name(scope_name)
{
  Start();
}

ScopeTimer::~ScopeTimer()
{
  if (!m_printed)
  {
    Stop();
    Print();
  }
}

void ScopeTimer::Start()
{
  m_start_time = clock::now();
  m_printed = false;
  m_started = true;
}

void ScopeTimer::Stop()
{
  m_end_time = clock::now();
  m_started = false;
}

void ScopeTimer::Print()
{
  if (m_started)
    Stop();

  const double ms = std::chrono::duration<double>(m_end_time - m_start_time).count() * 1000.0;
  std::fprintf(stderr, "%s took %.4f msec\n", m_scope_name.c_str(), ms);
  m_printed = true;
}
