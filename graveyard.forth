\ GRAVEYARD MAP — SNAPKITTYWEST/the-49th-call
\ 1 repos | rendered by AHMAD-BOT + Forth renderer
\ The graveyard in Forth. Every repo is a word.

\ ── the-49th-call (gravity: 0.2, status: orphan) ──
: crawl-the-49th-call ( -- )
  0.2 gravity
  dup alive? IF
    ." the-49th-call alive " cr
  ELSE dup broken? IF
    ." the-49th-call broken " cr
    "the-49th-call" repair
  ELSE
    ." the-49th-call orphan " cr
    "the-49th-call" flag
  THEN THEN
  drop
;

: crawl-graveyard ( -- )
  ." === SNAPKITTYWEST/the-49th-call GRAVEYARD CRAWL ===" cr
  crawl-the-49th-call
  ." === CRAWL COMPLETE ===" cr
;

crawl-graveyard