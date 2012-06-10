#pragma once

template<typename Iterator> Iterator SkipWhitespaces (Iterator it, Iterator end)
{
   for (; it != end; ++it) {
      if (*it !=' ' && *it != '\t') return it;
   }

   return end;
}

template<typename Iterator> Iterator ConsumeUntil (Iterator it, Iterator end, char until)
{
   Iterator itBegin = it;

   for (; it != end; ++it) {
      if (*it == until) return it;
   }

   return itBegin;
}

template<typename Iterator1, typename Iterator2> Iterator1 ConsumeIfEqual (Iterator1 it, Iterator1 end, Iterator2 compareIt, Iterator2 compareEnd)
{
   Iterator1 itStart = it;

   while (compareIt != compareEnd) {
      if (it == end) return itStart;
      if (*it != *compareIt) return itStart;

      ++compareIt;
      ++it;
   }

   return it;
}
