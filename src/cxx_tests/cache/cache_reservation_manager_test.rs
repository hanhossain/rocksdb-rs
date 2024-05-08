#[allow(dead_code)]
#[cxx::bridge(namespace = "rocksdb")]
mod ffi {
    unsafe extern "C++" {
        include!("cache/cache_reservation_manager_test.h");
        fn CacheReservationManagerTest_GenerateCacheKey_Test();
        fn CacheReservationManagerTest_KeepCacheReservationTheSame_Test();
        fn CacheReservationManagerTest_IncreaseCacheReservationByMultiplesOfDummyEntrySize_Test();
        fn CacheReservationManagerTest_IncreaseCacheReservationNotByMultiplesOfDummyEntrySize_Test();
        fn CacheReservationManager_IncreaseCacheReservationOnFullCache_Test();
        fn CacheReservationManagerTest_DecreaseCacheReservationByMultiplesOfDummyEntrySize_Test();
        fn CacheReservationManagerTest_DecreaseCacheReservationNotByMultiplesOfDummyEntrySize_Test();
        fn CacheReservationManagerWithDelayedDecreaseTest_DecreaseCacheReservationWithDelayedDecrease(
        );
        fn CacheReservationManagerDestructorTest_ReleaseRemainingDummyEntriesOnDestruction_Test();
        fn CacheReservationHandleTest_HandleTest_Test();
    }
}

#[allow(non_snake_case)]
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn CacheReservationManagerTest_GenerateCacheKey_Test() {
        ffi::CacheReservationManagerTest_GenerateCacheKey_Test();
    }

    #[test]
    fn CacheReservationManagerTest_KeepCacheReservationTheSame_Test() {
        ffi::CacheReservationManagerTest_KeepCacheReservationTheSame_Test();
    }

    #[test]
    fn CacheReservationManagerTest_IncreaseCacheReservationByMultiplesOfDummyEntrySize_Test() {
        ffi::CacheReservationManagerTest_IncreaseCacheReservationByMultiplesOfDummyEntrySize_Test();
    }

    #[test]
    fn CacheReservationManagerTest_IncreaseCacheReservationNotByMultiplesOfDummyEntrySize_Test() {
        ffi::CacheReservationManagerTest_IncreaseCacheReservationNotByMultiplesOfDummyEntrySize_Test();
    }

    #[test]
    fn CacheReservationManager_IncreaseCacheReservationOnFullCache_Test() {
        ffi::CacheReservationManager_IncreaseCacheReservationOnFullCache_Test();
    }

    #[test]
    fn CacheReservationManagerTest_DecreaseCacheReservationByMultiplesOfDummyEntrySize_Test() {
        ffi::CacheReservationManagerTest_DecreaseCacheReservationByMultiplesOfDummyEntrySize_Test();
    }

    #[test]
    fn CacheReservationManagerTest_DecreaseCacheReservationNotByMultiplesOfDummyEntrySize_Test() {
        ffi::CacheReservationManagerTest_DecreaseCacheReservationNotByMultiplesOfDummyEntrySize_Test();
    }

    #[test]
    fn CacheReservationManagerWithDelayedDecreaseTest_DecreaseCacheReservationWithDelayedDecrease()
    {
        ffi::CacheReservationManagerWithDelayedDecreaseTest_DecreaseCacheReservationWithDelayedDecrease();
    }

    #[test]
    fn CacheReservationManagerDestructorTest_ReleaseRemainingDummyEntriesOnDestruction_Test() {
        ffi::CacheReservationManagerDestructorTest_ReleaseRemainingDummyEntriesOnDestruction_Test();
    }

    #[test]
    fn CacheReservationHandleTest_HandleTest_Test() {
        ffi::CacheReservationHandleTest_HandleTest_Test();
    }
}
