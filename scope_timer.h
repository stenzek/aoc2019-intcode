#pragma once
#include <chrono>
#include <string>
#include <string_view>

class ScopeTimer
{
public:
  ScopeTimer(std::string_view scope_name);
  ~ScopeTimer();

  const std::string& GetScopeName() const { return m_scope_name; }
  void SetScopeName(std::string_view scope_name) { m_scope_name = scope_name; }

  void Start();
  void Stop();
  void Print();

private:
  using clock = std::chrono::steady_clock;

  clock::time_point m_start_time;
  clock::time_point m_end_time;
  std::string m_scope_name;
  bool m_started = false;
  bool m_printed = false;
};
