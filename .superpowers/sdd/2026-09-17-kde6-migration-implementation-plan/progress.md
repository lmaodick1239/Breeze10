# SDD ledger — plan: plans/2026-09-17-kde6-migration-implementation-plan.md

BASE: 748659fad836c320cbbf6f2737e2d7ec1729598b

Workspace setup complete. Stage 1 build/test baseline deferred as requested.
2026-09-17 Stage 1 review: Spec ❌; quality Issues (Critical); HEAD 734aa071043a209aebd9e4feae8d3a1894d3ece7; KCM split incomplete (exception-list/includes/XCB) and KDecoration install-namespace wiring inverted.
2026-09-17 Stage 1 fixes committed: 58ab580631e850a7127abe8e214f4f100e66a823; KCM split/include/XCB and decoration/KCM namespace wiring corrected.
2026-09-17 Stage 2 source port committed: 57e609a6375de5b6a609b0121d3b663b1204c1d2; client access localized in breezecompat.h and requested QRegularExpression/typed signal changes applied. Build unavailable because KF6/KDecoration dependencies were missing and build-kde6-stage1 was not generated. Stages 3 and 4 intentionally deferred.
2026-09-17 Stage 1 fix review: Spec ✅; quality Approved; prior KCM split, optional XCB linkage, and install-namespace findings addressed.
2026-09-17 Stage 2 review: Spec ❌; quality Issues (Important); KDecoration3 compatibility branch is type-inconsistent with the KDecoration2 decoration and signal call sites. See task-1-2-review.md. No CMake re-run; missing KF6/KDecoration packages remain an environment limitation.
Task 2: fix round 1/5 (KDecoration type family; commit f911c5755737df17b093cd70f7af3334af2e19d2)
Task 2: fix review (range 57e609a..f911c57; Spec ✅; quality Approved)
Task 2: complete (commits 57e609a..f911c57, review clean)
Task 3: implement (commit 7c6bb7fdfb2d29233fa98a2256f4b01ee4ceb6ca)
Task 3: review (range f911c57..7c6bb7f; Spec ❌; quality Issues)
Task 3: fix round 1/5 (default XCB screen; commit 321dd29)
Task 3: fix round 1/5 re-review (range 7c6bb7f..321dd29; Spec ❌; quality Issues; 0 addressed, 1 open)
Task 3: fix round 2/5 (DefaultScreen + no x11->screen(); commit pending)
