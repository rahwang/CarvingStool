;; Access package manager
(require 'package)

;; add user contributed packages
(add-to-list 'package-archives
    '("marmalade" .
      "http://marmalade-repo.org/packages/"))
(package-initialize)

;; Add the original Emacs Lisp Package Archive
(add-to-list 'package-archives
             '("elpa" . "http://tromey.com/elpa/"))

;; Add MELPA
(add-to-list 'package-archives
             '("melpa" . "http://melpa.milkbox.net/packages/") t)

;; Delete toolbar
(tool-bar-mode -1)

;; Nyan Cat!
(add-to-list 'load-path "~/.emacs.d/elpa/nyan-mode-20120710.1922/")
(require 'nyan-mode)
(nyan-mode t)
(nyan-start-animation)


;; No start-up page
(setq inhibit-startup-message t)

;; Create directory for backps
(setq backup-directory-alist `(("." . "~/.saves")))

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(ansi-color-names-vector ["black" "#d55e00" "#009e73" "#f8ec59" "#0072b2" "#cc79a7" "#56b4e9" "white"])
 '(custom-enabled-themes (quote (deeper-blue)))
 '(initial-buffer-choice t)
 '(initial-scratch-message ";; Scratch")
 '(word-wrap t))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )

;; enable InteractivelyDoThings mode
(require 'ido)
(ido-mode t)

;; Add colors to terminal emulator
(add-hook 'shell-mode-hook 'ansi-color-for-comint-mode-on)

;; Tab complete
(global-set-key (kbd "<backtab>") 'dabbrev-expand)

(defun comment-or-uncomment-region-or-line ()
    (interactive)
    (let (beg end)
        (if (region-active-p)
            (setq beg (region-beginning) end (region-end))
            (setq beg (line-beginning-position) end (line-end-position)))
        (comment-or-uncomment-region beg end)))

;; Change startup window size
(if window-system
    (set-frame-size (selected-frame) 80 50))

;; Comment-uncomment region or line
(global-set-key (kbd "C-x m") 'comment-or-uncomment-region-or-line)
