import AppKit
import WebKit

// A separate short-lived process keeps browser loading out of the GTK UI thread.
final class Reader: NSObject, WKNavigationDelegate {
    let webView: WKWebView = {
        let configuration = WKWebViewConfiguration()
        configuration.websiteDataStore = .nonPersistent()
        return WKWebView(frame: NSRect(x: 0, y: 0, width: 1024, height: 768), configuration: configuration)
    }()
    var window: NSWindow!
    var timer: Timer?
    var finished = false
    let script = #"""
    (() => {
      const match = location.pathname.match(/\/reel\/(\d+)/);
      const id = match ? match[1] : new URL(location.href).searchParams.get('v');
      if (!id) return null;
      let found = null;
      function walk(v) {
        if (!v || typeof v !== 'object') return;
        if (String(v.id) === id && (v.browser_native_hd_url || v.browser_native_sd_url))
          found = {id, hd: v.browser_native_hd_url, sd: v.browser_native_sd_url,
                   url: location.href,
                   title: document.querySelector('meta[property="og:title"]')?.content};
        for (const x of Object.values(v)) if (x && typeof x === 'object') walk(x);
      }
      for (const s of document.scripts) { try { walk(JSON.parse(s.textContent)); } catch {} }
      return found ? JSON.stringify(found) : null;
    })()
    """#

    func start(_ url: URL) {
        NSApplication.shared.setActivationPolicy(.prohibited)
        // Use a private session; no access to the user's browser cookies or profile.
        webView.customUserAgent = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.0 Safari/605.1.15"
        webView.navigationDelegate = self
        window = NSWindow(contentRect: webView.frame, styleMask: [.borderless], backing: .buffered, defer: false)
        window.contentView = webView
        webView.load(URLRequest(url: url, timeoutInterval: 35))
        timer = Timer.scheduledTimer(withTimeInterval: 0.5, repeats: true) { [weak self] _ in self?.inspect() }
        DispatchQueue.main.asyncAfter(deadline: .now() + 40) { [weak self] in
            self?.fail("Facebook 페이지에서 공개 영상 파일을 찾지 못했습니다.")
        }
    }

    func inspect() {
        guard !finished else { return }
        webView.evaluateJavaScript(script) { [weak self] value, _ in
            guard let self, !self.finished, let json = value as? String else { return }
            self.finished = true
            self.timer?.invalidate()
            print(json)
            fflush(stdout)
            exit(0)
        }
    }

    func fail(_ text: String) {
        guard !finished else { return }
        finished = true
        timer?.invalidate()
        FileHandle.standardError.write(Data((text + "\n").utf8))
        exit(1)
    }

    func webView(_ webView: WKWebView, didFailProvisionalNavigation navigation: WKNavigation!, withError error: Error) {
        fail(error.localizedDescription)
    }
    func webView(_ webView: WKWebView, didFail navigation: WKNavigation!, withError error: Error) {
        fail(error.localizedDescription)
    }
    func webView(_ webView: WKWebView, decidePolicyFor navigationAction: WKNavigationAction,
                 decisionHandler: @escaping (WKNavigationActionPolicy) -> Void) {
        let host = navigationAction.request.url?.host?.lowercased() ?? ""
        decisionHandler(host == "facebook.com" || host.hasSuffix(".facebook.com") ? .allow : .cancel)
    }
}

guard CommandLine.arguments.count == 2, let url = URL(string: CommandLine.arguments[1]),
      url.scheme == "https", let host = url.host?.lowercased(),
      host == "facebook.com" || host.hasSuffix(".facebook.com") else {
    FileHandle.standardError.write(Data("Expected an HTTPS Facebook reel URL\n".utf8))
    exit(2)
}
let app = NSApplication.shared
let reader = Reader()
reader.start(url)
app.run()
