
//
function fun_7_47093(event){
    const vggSdk = new _currentVgg.VggSdk();

    if (vggSdk.setEnv) {
        vggSdk.setEnv(_currentVggEnv);
    }

    switch (event.type) {
        
        case "click":{
            vggSdk.openUrl("https://github.com/verygoodgraphics/vgg_cli", "_blank");
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

export default fun_7_47093;