
D3D12_RESOURCE_BARRIER
D3D12Transition(ID3D12Resource *pResource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
    D3D12_RESOURCE_BARRIER TransitionBarrier = {};
    TransitionBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    TransitionBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    TransitionBarrier.Transition.pResource = pResource;
    TransitionBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    TransitionBarrier.Transition.StateBefore = StateBefore;
    TransitionBarrier.Transition.StateAfter = StateAfter;
    return TransitionBarrier;
}
