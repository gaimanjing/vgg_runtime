
//
function fun_1_124(event){
    const vggSdk = new _currentVgg.VggSdk();

    if (vggSdk.setEnv) {
        vggSdk.setEnv(_currentVggEnv);
    }

    switch (event.type) {
        
        case "mouseenter":{
            vggSdk.presentState(event.target, "1:124", "1:69", {"animation": {"type": "none", "timingFunction": "linear", "duration": 0.15000000596046448}, "resetScrollPosition": false});
            break;
        }
        case "mouseleave":{
            vggSdk.dismissState(event.target);
            break;
        }

        default:
            break;
    }

    if (vggSdk.delete) {
        vggSdk.delete();
    }

    if (event.delete) {
        event.delete();
    }
}

export default fun_1_124;
